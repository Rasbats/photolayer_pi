/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  PhotoLayer Plugin - Transverse Mercator / UTM without PROJ
 *
 * Header-only, no wxWidgets or libgeotiff dependency.
 *
 * Forward and inverse Transverse Mercator use the Kruger n-series (as given
 * by Karney, 2011), truncated at n^4. For WGS84 (n = 0.00168) the truncation
 * error is well below a millimetre, and the series stays accurate far
 * outside the nominal 6 degree UTM zone width.
 *
 * Only WGS84-compatible datums are supported (WGS84, ETRS89, NAD83). The
 * ellipsoid is always WGS84; GRS80 differs by about 0.1 mm in the semi-minor
 * axis. NAD83 vs WGS84 can differ by up to ~2 m on the ground (datum, not
 * ellipsoid) and no shift is applied.
 *
 ***************************************************************************
 */

#ifndef PHOTOLAYER_TRANSVERSE_MERCATOR_H
#define PHOTOLAYER_TRANSVERSE_MERCATOR_H

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace PhotoLayerTM {

struct Params {
    double cm_deg;   /* central meridian, degrees */
    double k0;       /* scale factor on the central meridian */
    double fe, fn;   /* false easting / northing, metres */
    Params() : cm_deg(0.0), k0(0.9996), fe(500000.0), fn(0.0) {}
};

const double WGS84_A = 6378137.0;
const double WGS84_INV_F = 298.257223563;

/* Fill UTM parameters from an EPSG projected CRS code, with no lookup
   tables. Returns false for any code that is not a supported UTM zone:
     326xx / 327xx  WGS 84 / UTM zone xxN / xxS
     25828-25838    ETRS89 / UTM zone 28N-38N
     26901-26923    NAD83 / UTM zone 1N-23N  */
inline bool UTMFromEPSG(int pcs, Params &p)
{
    int zone = 0;
    bool south = false;
    if (pcs >= 32601 && pcs <= 32660)      zone = pcs - 32600;
    else if (pcs >= 32701 && pcs <= 32760) { zone = pcs - 32700; south = true; }
    else if (pcs >= 25828 && pcs <= 25838) zone = pcs - 25800;
    else if (pcs >= 26901 && pcs <= 26923) zone = pcs - 26900;
    else return false;

    p.cm_deg = zone * 6.0 - 183.0;
    p.k0 = 0.9996;
    p.fe = 500000.0;
    p.fn = south ? 10000000.0 : 0.0;
    return true;
}

/* Spherical Mercator Y for a latitude in radians, and its inverse. This is
   the same relation PhotoLayerImageCoordinates::lat() assumes. */
inline double MercY(double lat_rad)
{
    return std::log(std::tan(M_PI / 4.0 + lat_rad / 2.0));
}

inline double MercLat(double y)
{
    return 2.0 * std::atan(std::exp(y)) - M_PI / 2.0;
}

struct Consts {
    double n, A;
    double alpha[5], beta[5], delta[5];   /* index 1..4 used */

    Consts()
    {
        double f = 1.0 / WGS84_INV_F;
        n = f / (2.0 - f);
        double n2 = n * n, n3 = n2 * n, n4 = n3 * n;

        A = WGS84_A / (1.0 + n) * (1.0 + n2 / 4.0 + n4 / 64.0);

        alpha[0] = 0.0;
        alpha[1] = n / 2.0 - 2.0 * n2 / 3.0 + 5.0 * n3 / 16.0 + 41.0 * n4 / 180.0;
        alpha[2] = 13.0 * n2 / 48.0 - 3.0 * n3 / 5.0 + 557.0 * n4 / 1440.0;
        alpha[3] = 61.0 * n3 / 240.0 - 103.0 * n4 / 140.0;
        alpha[4] = 49561.0 * n4 / 161280.0;

        beta[0] = 0.0;
        beta[1] = n / 2.0 - 2.0 * n2 / 3.0 + 37.0 * n3 / 96.0 - n4 / 360.0;
        beta[2] = n2 / 48.0 + n3 / 15.0 - 437.0 * n4 / 1440.0;
        beta[3] = 17.0 * n3 / 480.0 - 37.0 * n4 / 840.0;
        beta[4] = 4397.0 * n4 / 161280.0;

        delta[0] = 0.0;
        delta[1] = 2.0 * n - 2.0 * n2 / 3.0 - 2.0 * n3 + 116.0 * n4 / 45.0;
        delta[2] = 7.0 * n2 / 3.0 - 8.0 * n3 / 5.0 - 227.0 * n4 / 45.0;
        delta[3] = 56.0 * n3 / 15.0 - 136.0 * n4 / 35.0;
        delta[4] = 4279.0 * n4 / 630.0;
    }
};

inline const Consts &GetConsts()
{
    static const Consts c;
    return c;
}

/* lat/lon (degrees, WGS84) -> easting/northing (metres) */
inline void Forward(double lat_deg, double lon_deg, const Params &p,
                    double &E, double &N)
{
    const Consts &c = GetConsts();

    double phi = lat_deg * M_PI / 180.0;
    double dl = lon_deg - p.cm_deg;
    while (dl > 180.0)  dl -= 360.0;
    while (dl < -180.0) dl += 360.0;
    double lam = dl * M_PI / 180.0;

    double sphi = std::sin(phi);
    double q = 2.0 * std::sqrt(c.n) / (1.0 + c.n);
    double t = std::sinh(std::atanh(sphi) - q * std::atanh(q * sphi));

    double xi_p  = std::atan2(t, std::cos(lam));
    double eta_p = std::atanh(std::sin(lam) / std::sqrt(1.0 + t * t));

    double xi = xi_p, eta = eta_p;
    for (int j = 1; j <= 4; j++) {
        xi  += c.alpha[j] * std::sin(2.0 * j * xi_p) * std::cosh(2.0 * j * eta_p);
        eta += c.alpha[j] * std::cos(2.0 * j * xi_p) * std::sinh(2.0 * j * eta_p);
    }

    E = p.fe + p.k0 * c.A * eta;
    N = p.fn + p.k0 * c.A * xi;
}

/* easting/northing (metres) -> lat/lon (degrees, WGS84) */
inline void Inverse(double E, double N, const Params &p,
                    double &lat_deg, double &lon_deg)
{
    const Consts &c = GetConsts();

    double xi  = (N - p.fn) / (p.k0 * c.A);
    double eta = (E - p.fe) / (p.k0 * c.A);

    double xi_p = xi, eta_p = eta;
    for (int j = 1; j <= 4; j++) {
        xi_p  -= c.beta[j] * std::sin(2.0 * j * xi) * std::cosh(2.0 * j * eta);
        eta_p -= c.beta[j] * std::cos(2.0 * j * xi) * std::sinh(2.0 * j * eta);
    }

    double chi = std::asin(std::sin(xi_p) / std::cosh(eta_p));
    double phi = chi;
    for (int j = 1; j <= 4; j++)
        phi += c.delta[j] * std::sin(2.0 * j * chi);

    double lam = std::atan2(std::sinh(eta_p), std::cos(xi_p));

    lat_deg = phi * 180.0 / M_PI;
    lon_deg = p.cm_deg + lam * 180.0 / M_PI;
}

/* Output layout of a warped image: an image whose columns are linear in
   longitude and whose rows are linear in spherical Mercator Y, which is
   what PhotoLayerImageCoordinates::lat()/lon() assume. */
struct Layout {
    int offx, offy;            /* added to the warped x/y so the minimum is >= 0 */
    int mw, mh;                /* output size in pixels */
    double lat1, lon1;         /* geographic position of output pixel (0,0) */
    double lat2, lon2;         /* geographic position of output pixel (mw,mh) */
    double clat, clon;         /* centre */
};

/* Maps source pixels (corner-based, (0,0) = top-left corner of the image)
   of a UTM / Transverse Mercator raster to and from the Mercator working
   space used by PhotoLayerImage. */
struct Warp {
    Params tm;
    double e0, a, b, n0, c, d;   /* E = e0 + a*px + b*py ; N = n0 + c*px + d*py */
    double S;                    /* output pixels per radian, set by Init() */

    Warp() : e0(0), a(0), b(0), n0(0), c(0), d(0), S(0) {}

    void PixelToEN(double px, double py, double &E, double &N) const
    {
        E = e0 + a * px + b * py;
        N = n0 + c * px + d * py;
    }

    bool ENToPixel(double E, double N, double &px, double &py) const
    {
        double det = a * d - b * c;
        if (det == 0.0)
            return false;
        double dE = E - e0, dN = N - n0;
        px = (d * dE - b * dN) / det;
        py = (a * dN - c * dE) / det;
        return true;
    }

    void PixelToLatLon(double px, double py, double &lat, double &lon) const
    {
        double E, N;
        PixelToEN(px, py, E, N);
        Inverse(E, N, tm, lat, lon);
    }

    /* Choose S so one output pixel covers the same ground distance as one
       source pixel at the image centre (Mercator is conformal, so this holds
       in both directions). Returns false for a degenerate transform. */
    bool Init(int w, int h)
    {
        double lat, lon;
        PixelToLatLon(w / 2.0, h / 2.0, lat, lon);
        double gs = 0.5 * (std::sqrt(a * a + c * c) + std::sqrt(b * b + d * d));
        if (!(gs > 0.0) || !std::isfinite(gs) || !std::isfinite(lat))
            return false;
        S = WGS84_A * std::cos(lat * M_PI / 180.0) / gs;
        return std::isfinite(S) && S > 0.0;
    }

    /* source pixel -> warped x/y, before the size multiplier and offsets */
    void PixelToMerc(double px, double py, double &x, double &y) const
    {
        double lat, lon;
        PixelToLatLon(px, py, lat, lon);
        x = lon * M_PI / 180.0 * S;
        y = -MercY(lat * M_PI / 180.0) * S;     /* rows increase southwards */
    }

    /* exact inverse of PixelToMerc */
    void MercToPixel(double x, double y, double &px, double &py) const
    {
        double lon = x / S * 180.0 / M_PI;
        double lat = MercLat(-y / S) * 180.0 / M_PI;
        double E, N;
        Forward(lat, lon, tm, E, N);
        if (!ENToPixel(E, N, px, py))
            px = py = std::nan("");
    }

    /* Bounding box of the warped footprint. The grid is rotated relative to
       true north and its edges are curved in lat/lon, so the edges are
       sampled rather than trusting the four corners alone. */
    bool ComputeLayout(int w, int h, double mult, double ratio, Layout &L) const
    {
        if (!(S > 0.0) || !(mult > 0.0) || !(ratio > 0.0) || w < 1 || h < 1)
            return false;
        const double ky = mult / ratio;

        double minx = 1e300, maxx = -1e300, miny = 1e300, maxy = -1e300;
        const int STEPS = 16;
        for (int i = 0; i <= STEPS; i++) {
            double t = (double)i / STEPS;
            double pts[4][2] = { { w * t, 0.0 }, { w * t, (double)h },
                                 { 0.0, h * t }, { (double)w, h * t } };
            for (int k = 0; k < 4; k++) {
                double x, y;
                PixelToMerc(pts[k][0], pts[k][1], x, y);
                x *= mult;
                y *= ky;
                if (x < minx) minx = x;
                if (x > maxx) maxx = x;
                if (y < miny) miny = y;
                if (y > maxy) maxy = y;
            }
        }
        if (!std::isfinite(minx) || !std::isfinite(maxx) ||
            !std::isfinite(miny) || !std::isfinite(maxy))
            return false;

        L.offx = (int)std::ceil(-minx);
        L.offy = (int)std::ceil(-miny);
        L.mw = (int)std::ceil(maxx + L.offx);
        L.mh = (int)std::ceil(maxy + L.offy);
        if (L.mw < 1 || L.mh < 1)
            return false;

        L.lon1 = (0.0 - L.offx) / (mult * S) * 180.0 / M_PI;
        L.lon2 = ((double)L.mw - L.offx) / (mult * S) * 180.0 / M_PI;
        L.lat1 = MercLat(-(0.0 - L.offy) / (ky * S)) * 180.0 / M_PI;
        L.lat2 = MercLat(-((double)L.mh - L.offy) / (ky * S)) * 180.0 / M_PI;
        L.clon = ((L.mw / 2.0) - L.offx) / (mult * S) * 180.0 / M_PI;
        L.clat = MercLat(-((L.mh / 2.0) - L.offy) / (ky * S)) * 180.0 / M_PI;
        return true;
    }
};

} /* namespace PhotoLayerTM */

#endif
