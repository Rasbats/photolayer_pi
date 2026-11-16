/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  PhotoLayer Plugin
 * Author:   Mike Rossiter, based on WeatherFax by Sean D'Epagnier
 *
 ***************************************************************************
 *   Copyright (C) 2017 by Mike Rossiter                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************
 *
 * CHANGES IN THIS REVISION - see PhotoLayer.h for a summary. Search for
 * "FIX:" comments below for each individual change and why it was made.
 *
 ***************************************************************************
 */

#include <list>
#include "tinyxml.h"
#include "PhotoLayer_pi.h"
#include "PhotoLayerImage.h"
#include "PhotoLayer.h"
#include "AboutDialog.h"
#include "icons.h"
#include "georef.h"
#include "wximgkap.h"

/* XPM */
static const char *photolayer_xpm[] = {
	"32 32 4 1 0 0",
	"  c #E6E6E6",
	"! c #FF0080",
	"# c #00FFFF",
	"$ c None",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"          !!!!!!!!!!!!!!!!!!!!!!",
	"         !!!!!!!!!!!!!!!!!!!!!!!",
	"        !!!!!!!!!!!!!!!!!!!!!!! ",
	"       !!!!!!!!!!!!!!!!!!!!!!!  ",
	"      !!!!!!!!!!!!!!!!!!!!!!!   ",
	"     !!!!!!!!!!!!!!!!!!!!!!!    ",
	"    !!!!!!!!!!!!!!!!!!!!!!!     ",
	"   !!!!!!!!!!!!!!!!!!!!!!!      ",
	"  !!!!!!!!!!!!!!!!!!!!!!!       ",
	" !!!!!!!!!!!!!!!!!!!!!!!        ",
	"!!!!!!!!!!!!!!!!!!!!!!!         ",
	"!!!!!!!!!!!!!!!!!!!!!!          ",
	"         #######################",
	"        ########################",
	"       ######################## ",
	"      ########################  ",
	"     ########################   ",
	"    ########################    ",
	"   ########################     ",
	"  ########################      ",
	" ########################       ",
	"########################        ",
	"#######################         ",
	"                                ",
	"                                ",
	"                                ",
	"                                "
};

/*
* Report the file(s) corner coordinates in projected coordinates, and
* if possible lat/long.
*
* FIX: this function now:
*   1. Accepts an explicit 'index' into m_geoPolygon instead of relying on
*      a shared, externally-incremented counter (cornerNum). The old
*      counter only advanced correctly when every single corner succeeded;
*      a single failed corner shifted every later write by one slot and
*      silently corrupted the array. Fixed indices remove that failure
*      mode entirely.
*   2. Handles ModelTypeProjected as well as ModelTypeGeographic. The
*      original code returned FALSE immediately for any projected CRS
*      (e.g. Web Mercator, World Mercator), meaning corner extraction
*      always failed for Mercator-projected GeoTIFFs.
*   3. Converts projected coordinates to lon/lat without depending on
*      GTIFProj4ToLatLong() (which needs PROJ.4 linked into libgeotiff
*      plus populated EPSG CSV tables - neither of which can be assumed
*      present). Instead it reads the false easting/northing and central
*      meridian directly from the file's own GeoKeys and applies a closed
*      form spherical (EPSG:3857/3785) or ellipsoidal (e.g. EPSG:3395)
*      Mercator inverse, falling back to hardcoded WGS84 ellipsoid
*      constants when the file's SemiMajor/SemiMinor come back as 0
*      (again, a missing-CSV symptom).
*/

int PhotoLayer::GTIFReportACorner(GTIF *gtif, GTIFDefn *defn,
	const char * corner_name,
	double x, double y, int inv_flag, int dec_flag, int index)

{
	if (!GTIFImageToPCS(gtif, &x, &y))
		return FALSE;

	if (defn->Model == ModelTypeGeographic)
	{
		/* x,y are already lon,lat degrees - nothing to do */
	}
	else if (defn->Model == ModelTypeProjected)
	{
		/* WGS84 ellipsoid constants, used whenever the file's own ellipsoid
		   parameters are unavailable (SemiMajor/SemiMinor == 0), which
		   happens when libgeotiff's EPSG CSV tables aren't present. */
		const double WGS84_A = 6378137.0;
		const double WGS84_B = 6356752.314245179;

		double falseEasting = 0, falseNorthing = 0, lon0 = 0;
		GTIFKeyGet(gtif, ProjFalseEastingGeoKey, &falseEasting, 0, 1);
		GTIFKeyGet(gtif, ProjFalseNorthingGeoKey, &falseNorthing, 0, 1);
		GTIFKeyGet(gtif, ProjNatOriginLongGeoKey, &lon0, 0, 1);

		double dx = x - falseEasting;
		double dy = y - falseNorthing;

		double a = (defn->SemiMajor > 0) ? defn->SemiMajor : WGS84_A;
		double b = (defn->SemiMinor > 0) ? defn->SemiMinor : WGS84_B;

		double e2 = 0.0;
		if (defn->PCS != 3857 && defn->PCS != 3785)   /* those two are spherical by spec */
			e2 = 1.0 - (b*b)/(a*a);
		double e = sqrt(e2);

		double lon = lon0 + (dx / a) * (180.0 / M_PI);

		double t = exp(-dy / a);
		double phi = M_PI/2.0 - 2.0*atan(t);
		for (int i = 0; i < 6; i++)   /* converges to well under 1e-9 rad in a handful of iterations */
			phi = M_PI/2.0 - 2.0*atan(t * pow((1 - e*sin(phi))/(1 + e*sin(phi)), e/2.0));

		x = lon;
		y = phi * (180.0 / M_PI);
	}
	else
	{
		return FALSE;   /* e.g. Geocentric - unsupported */
	}

	if (!dec_flag)
		return FALSE;

	m_geoPolygon[index]     = x;
	m_geoPolygon[index + 1] = y;
	return true;
}

void PhotoLayer::GTIFPrintCorners(GTIF *gtif, GTIFDefn *defn,
	int xsize, int ysize, int inv_flag, int dec_flag)
{
	/* FIX: clear the array first so a failed corner never leaves data from
	   a previously opened file sitting in these slots. */
	memset(m_geoPolygon, 0, sizeof(m_geoPolygon));

	struct { const char *name; double x, y; int idx; } corners[5] = {
		{ "Upper Left",  0.0,               0.0,               0 },
		{ "Lower Left",  0.0,               (double)ysize,     2 },
		{ "Upper Right", (double)xsize,     0.0,               4 },
		{ "Lower Right", (double)xsize,     (double)ysize,     6 },
		{ "Center",      xsize / 2.0,       ysize / 2.0,       8 },
	};

	for (int i = 0; i < 5; i++) {
		if (!GTIFReportACorner(gtif, defn, corners[i].name,
		                        corners[i].x, corners[i].y, inv_flag, dec_flag, corners[i].idx)) {
			wxLogMessage(wxString::Format("GTIFPrintCorners: corner '%s' failed", corners[i].name));
			if (i == 0) {
				wxMessageBox(_(" ... unable to transform points between pixel/line and PCS space\n"));
				return;
			}
			/* corner slot stays zeroed (from the memset above) rather than
			   corrupting a later index */
		}
	}
}



double AttributeDouble(TiXmlElement *e, const char *name, double def)
{
    const char *attr = e->Attribute(name);
    if(!attr)
        return def;
    char *end;
    double d = strtod(attr, &end);
    if(end == attr)
        return def;
    return d;
}

int AttributeInt(TiXmlElement *e, const char *name, int def)
{
    const char *attr = e->Attribute(name);
    if(!attr)
        return def;
    char *end;
    long d = strtol(attr, &end, 10);
    if(end == attr)
        return def;
    return d;
}

#define FAIL(X) do { error = X; goto failed; } while(0)

/* FIX: reads a .tfw/.tifw/.wld sidecar world file when the embedded GeoTIFF
   tags don't provide usable georeferencing (missing, or present but
   all-zero - seen with some SASPlanet exports). World-file line order is
   A, D, B, E, C, F (pixel-size-x, rotation, rotation, pixel-size-y,
   origin-x, origin-y). Returns false if no sidecar file is found. */
bool PhotoLayer::ReadWorldFile(wxString filename, double &A, double &Bp, double &Cp,
                                double &D, double &E, double &F)
{
	wxFileName fn(filename);
	wxString candidates[3] = {
		fn.GetPathWithSep() + fn.GetName() + _T(".tfw"),
		fn.GetPathWithSep() + fn.GetName() + _T(".tifw"),
		fn.GetPathWithSep() + fn.GetName() + _T(".wld")
	};

	for (int i = 0; i < 3; i++) {
		if (!wxFileExists(candidates[i]))
			continue;

		wxTextFile tf(candidates[i]);
		if (!tf.Open())
			continue;

		if (tf.GetLineCount() < 6) {
			tf.Close();
			continue;
		}

		double vals[6];
		for (int j = 0; j < 6; j++)
			vals[j] = wxAtof(tf.GetLine(j));

		A = vals[0]; D = vals[1]; Bp = vals[2]; E = vals[3]; Cp = vals[4]; F = vals[5];
		tf.Close();
		return true;
	}
	return false;
}

void PhotoLayer::LoadCoordinatesFromTIF(PhotoLayerImageCoordinateList &coords, wxString filename)
{
	wxString name = filename;

	PhotoLayerImageCoordinates* coord = new PhotoLayerImageCoordinates(name);
	coord->name = filename;
	coord->p1.x = 0;
	coord->p1.y = 0;
	coord->lat1 = m_geoPolygon[1];
	coord->lon1 = m_geoPolygon[0];

	coord->p2.x = imageWidthX;
	coord->p2.y = imageHeightY;

	coord->lat2 = m_geoPolygon[7];
	coord->lon2 = m_geoPolygon[6];

	coord->CenterLat = m_geoPolygon[9];
	coord->CenterLon = m_geoPolygon[8];

	coord->rotation = PhotoLayerImageCoordinates::NONE;

	/* FIX: pick the mapping type based on the file's own CRS model rather
	   than always assuming MERCATOR. A plain geographic (lat/lon) GeoTIFF
	   has pixel rows linear in *latitude degrees*, which PLATECARREE
	   models correctly; a projected (e.g. Web/World Mercator) GeoTIFF's
	   pixel rows are already linear in Mercator Y, which is what MERCATOR
	   assumes. Getting this wrong silently distorts the image vertically. */
	coord->mapping = (m_ModelType == ModelTypeGeographic)
		? PhotoLayerImageCoordinates::PLATECARREE
		: PhotoLayerImageCoordinates::MERCATOR;

	/* UTM / Transverse Mercator: record the projection and affine. The
	   corner lat/lons above are only provisional; MakeMappedImage() derives
	   the real ones (and p1/p2) from the warped layout. */
	if (m_isTM) {
		coord->mapping = PhotoLayerImageCoordinates::TRANSVERSE;
		coord->tmCm = m_tm.cm_deg;
		coord->tmK0 = m_tm.k0;
		coord->tmFE = m_tm.fe;
		coord->tmFN = m_tm.fn;
		for (int i = 0; i < 6; i++)
			coord->tmAff[i] = m_aff[i];
	}

	coord->inputtrueratio = 1.0;
	coord->mappingmultiplier = 1.0;
	coord->mappingratio = 1.0;

	coords.Append(coord);

}

void PhotoLayer::SaveTIFCoordinatesToXml(PhotoLayerImageCoordinateList &coords, wxString filename)
{
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "utf-8", "");
	doc.LinkEndChild(decl);

	TiXmlElement * root = new TiXmlElement("PhotoLayerDataSet");
	doc.LinkEndChild(root);

	for (unsigned int i = 0; i<coords.GetCount(); i++) {
		TiXmlElement *c = new TiXmlElement("Data");

		c->SetAttribute("Name", coords[i]->name.mb_str());

		c->SetAttribute("X1", wxString::Format(_T("%d"), coords[i]->p1.x).mb_str());
		c->SetAttribute("Y1", wxString::Format(_T("%d"), coords[i]->p1.y).mb_str());
		c->SetAttribute("Lat1", wxString::Format(_T("%.9f"), coords[i]->lat1).mb_str());
		c->SetAttribute("Lon1", wxString::Format(_T("%.9f"), coords[i]->lon1).mb_str());

		c->SetAttribute("X2", wxString::Format(_T("%d"), coords[i]->p2.x).mb_str());
		c->SetAttribute("Y2", wxString::Format(_T("%d"), coords[i]->p2.y).mb_str());
		c->SetAttribute("Lat2", wxString::Format(_T("%.9f"), coords[i]->lat2).mb_str());
		c->SetAttribute("Lon2", wxString::Format(_T("%.9f"), coords[i]->lon2).mb_str());
		c->SetAttribute("CenterLat", wxString::Format(_T("%.9f"), coords[i]->CenterLat).mb_str());
		c->SetAttribute("CenterLon", wxString::Format(_T("%.9f"), coords[i]->CenterLon).mb_str());

		/* The mapping type was never saved before, so every entry
		   reloaded as Mercator. */
		c->SetAttribute("Map", PhotoLayerImageCoordinates::MapName(coords[i]->mapping).ToUTF8());
		if (coords[i]->mapping == PhotoLayerImageCoordinates::TRANSVERSE) {
			c->SetAttribute("TmCm", wxString::Format(_T("%.12g"), coords[i]->tmCm).mb_str());
			c->SetAttribute("TmK0", wxString::Format(_T("%.12g"), coords[i]->tmK0).mb_str());
			c->SetAttribute("TmFE", wxString::Format(_T("%.12g"), coords[i]->tmFE).mb_str());
			c->SetAttribute("TmFN", wxString::Format(_T("%.12g"), coords[i]->tmFN).mb_str());
			static const char *affNames[6] = { "TmE0", "TmA", "TmB", "TmN0", "TmC", "TmD" };
			for (int k = 0; k < 6; k++)
				c->SetAttribute(affNames[k], wxString::Format(_T("%.17g"), coords[i]->tmAff[k]).mb_str());
		}

		root->LinkEndChild(c);
	}

	wxString layer_path = PhotoLayer_pi::StandardPath();
	wxString s = wxFileName::GetPathSeparator();

	if (!doc.SaveFile((layer_path + s + filename).mb_str()))
		wxLogMessage(_("PhotoLayer") + wxString(_T(": ")) + _("Failed to save xml file: ") + filename);
}

void PhotoLayer::LoadTIFCoordinatesFromXml(PhotoLayerImageCoordinateList &coords, wxString coordinatesets)
{
    TiXmlDocument doc;
	wxString name;
    wxString error;
    wxString coordinatesets_path = PhotoLayer_pi::StandardPath();
	wxString s = wxFileName::GetPathSeparator();

    if(!doc.LoadFile((coordinatesets_path + s + coordinatesets).mb_str()))
        FAIL(_("Failed to load data sets"));
    else {
		TiXmlElement* root = doc.RootElement();

        if(strcmp(root->Value(), "PhotoLayerDataSet"))
            FAIL(_("Invalid xml file"));

		for (TiXmlElement* e = root->FirstChildElement(); e; e = e->NextSiblingElement()) {
			if (!strcmp(e->Value(), "Data")) {
				name = wxString::FromUTF8(e->Attribute("Name"));				
				PhotoLayerImageCoordinates *coord = new PhotoLayerImageCoordinates(name);
				coord->name = name;
				coord->p1.x = AttributeInt(e, "X1", 0);
				coord->p1.y = AttributeInt(e, "Y1", 0);
				coord->lat1 = AttributeDouble(e, "Lat1", 0);
				coord->lon1 = AttributeDouble(e, "Lon1", 0);

				coord->p2.x = AttributeInt(e, "X2", 0);
				coord->p2.y = AttributeInt(e, "Y2", 0);
				coord->lat2 = AttributeDouble(e, "Lat2", 0);
				coord->lon2 = AttributeDouble(e, "Lon2", 0);

				coord->CenterLat = AttributeDouble(e, "CenterLat", 0);
				coord->CenterLon = AttributeDouble(e, "CenterLon", 0);

				/* entries written before the Map attribute existed stay Mercator */
				const char *mapattr = e->Attribute("Map");
				if (mapattr)
					coord->mapping = PhotoLayerImageCoordinates::GetMapType(wxString::FromUTF8(mapattr));
				if (coord->mapping == PhotoLayerImageCoordinates::TRANSVERSE) {
					coord->tmCm = AttributeDouble(e, "TmCm", 0);
					coord->tmK0 = AttributeDouble(e, "TmK0", 0.9996);
					coord->tmFE = AttributeDouble(e, "TmFE", 500000.0);
					coord->tmFN = AttributeDouble(e, "TmFN", 0);
					static const char *affNames[6] = { "TmE0", "TmA", "TmB", "TmN0", "TmC", "TmD" };
					for (int k = 0; k < 6; k++)
						coord->tmAff[k] = AttributeDouble(e, affNames[k], 0);
				}

				coords.Append(coord);

			}
			else
				FAIL(_("Unrecognized xml node: ") + wxString::FromUTF8(e->Value()));
		}
    }
    return;
failed:
    wxLogMessage(_("PhotoLayer") + wxString(_T(" : ")) + error);
}

PhotoLayer::PhotoLayer( PhotoLayer_pi &_PhotoLayer_pi, wxWindow* parent)
    : PhotoLayerBase( parent ),
      m_PhotoLayer_pi(_PhotoLayer_pi)
{
	
	LoadTIFCoordinatesFromXml(m_BuiltinCoords, _T("PhotoLayerDataSets.xml"));
	ShowSavedImages();
    UpdateMenuStates();

	EnableDisplayControls(true);
	m_sTransparency->SetValue(0);
	m_sWhiteTransparency->SetValue(0);
	m_cInvert->SetValue(false);

	return;
}

PhotoLayer::~PhotoLayer()
{
    for(unsigned int i=0; i<m_Faxes.size(); i++)
        delete m_Faxes[i];
}

void PhotoLayer::OnClose(wxCloseEvent& event) {

	m_PhotoLayer_pi.OnDialogClose();
}


void PhotoLayer::EnableDisplayControls(bool enable)
{
    m_sTransparency->Enable(enable);
    m_sWhiteTransparency->Enable(enable);
    m_cInvert->Enable(enable);
}

void PhotoLayer::ShowSavedImages(){

	int transparency = m_sTransparency->GetValue();
	int whitetransparency = m_sWhiteTransparency->GetValue();
	bool invert = m_cInvert->GetValue();

	wxImage wimg;
	wxString name;

	for (unsigned int i = 0; i < m_BuiltinCoords.GetCount(); i++){
		name = m_BuiltinCoords[i]->name;

		/* FIX: skip (rather than attempt to load and pop a modal error
		   dialog for) any entry whose file no longer exists on disk. */
		if (!wxFileExists(name)) {
			wxLogMessage("ShowSavedImages: skipping missing file " + name);
			continue;
		}

		if (!wimg.LoadFile(name)) {
			{
				wxMessageDialog mdlg(this, _("Failed to load input file: ") + name,
					_("PhotoLayer"), wxOK | wxICON_ERROR);
				mdlg.ShowModal();
				continue;   /* FIX: one bad file shouldn't abort loading the rest */
			}
		}
		PhotoLayerImage *img = new PhotoLayerImage(wimg, transparency, whitetransparency, invert);
		img->m_Coords = m_BuiltinCoords[i];
		img->MakePhasedImage();
		if (img->MakeMappedImage(this)){
			m_lFaxes->Append(name);
			m_Faxes.push_back(img);

		}
		else {
			delete img;   /* FIX: don't leak a PhotoLayerImage whose mapping failed */
		}
	}

	/* FIX: SetSelection()/Select() on an empty (or, for a multi-select
	   wxLB_EXTENDED listbox, wrongly-called) listbox is undefined/unsafe on
	   some platforms. Guard the count, and use Select() rather than
	   SetSelection() - see the note on Select() below in OpenImage(). */
	if (m_lFaxes->GetCount() > 0)
		m_lFaxes->Select(0);

	RequestRefresh(m_parent);
}

void PhotoLayer::OnFaxes( wxCommandEvent& event )
{
    UpdateMenuStates();

    /* take first selection.. we could do an average for sliders, but whatever */
    int selection;
    for(selection = 0; !m_lFaxes->IsSelected(selection); selection++)
        if(selection == (int)m_Faxes.size())
            return;

    PhotoLayerImage &img = *m_Faxes[selection];
    m_sTransparency->SetValue(img.m_iTransparency);
    m_sWhiteTransparency->SetValue(img.m_iWhiteTransparency);
    m_cInvert->SetValue(img.m_bInvert);

    RequestRefresh( m_parent );
}


/* UTM / Transverse Mercator georeferencing, with no PROJ and no EPSG tables.
   Fills m_tm and m_aff from the file's own GeoKeys and tie-point/scale tags
   (via GTIFImageToPCS, which only needs the raw tags), and fills
   m_geoPolygon from the warp. On failure m_Error says why. */
bool PhotoLayer::ReadTransverse(GTIF *gtif, GTIFDefn *defn, int xsize, int ysize)
{
	using namespace PhotoLayerTM;
	Params p;

	if (UTMFromEPSG(defn->PCS, p)) {
		/* standard UTM zone from the EPSG code alone */
	}
	else if (defn->PCS == 32767 /* user-defined */) {
		unsigned short ct = 0;
		GTIFKeyGet(gtif, ProjCoordTransGeoKey, &ct, 0, 1);
		if (ct != CT_TransverseMercator) {
			m_Error = _("Unsupported user-defined projection (only Mercator and Transverse Mercator/UTM are handled)");
			return false;
		}
		double lon0 = 0, lat0 = 0, k0 = 0, fe = 0, fn = 0;
		if (!GTIFKeyGet(gtif, ProjNatOriginLongGeoKey, &lon0, 0, 1))
			GTIFKeyGet(gtif, ProjCenterLongGeoKey, &lon0, 0, 1);
		if (!GTIFKeyGet(gtif, ProjNatOriginLatGeoKey, &lat0, 0, 1))
			lat0 = 0;
		if (!GTIFKeyGet(gtif, ProjScaleAtNatOriginGeoKey, &k0, 0, 1) || !(k0 > 0)) {
			m_Error = _("Transverse Mercator file has no scale factor (ProjScaleAtNatOrigin)");
			return false;
		}
		GTIFKeyGet(gtif, ProjFalseEastingGeoKey, &fe, 0, 1);
		GTIFKeyGet(gtif, ProjFalseNorthingGeoKey, &fn, 0, 1);
		if (fabs(lat0) > 1e-9) {
			m_Error = _("Transverse Mercator with a non-zero latitude of origin is not supported");
			return false;
		}
		/* The ellipsoid is always taken as WGS84; refuse a datum we know is not. */
		unsigned short gcs = 0;
		if (GTIFKeyGet(gtif, GeographicTypeGeoKey, &gcs, 0, 1) &&
		    gcs != 4326 && gcs != 4258 && gcs != 4269 && gcs != 32767) {
			m_Error = wxString::Format(_("Unsupported geographic CRS EPSG:%d (only WGS84 / ETRS89 / NAD83 are handled)"), (int)gcs);
			return false;
		}
		p.cm_deg = lon0; p.k0 = k0; p.fe = fe; p.fn = fn;
	}
	else {
		m_Error = wxString::Format(_("Unsupported projected CRS EPSG:%d"), (int)defn->PCS);
		return false;
	}

	/* the maths here assumes metres */
	unsigned short units = 0;
	if (GTIFKeyGet(gtif, ProjLinearUnitsGeoKey, &units, 0, 1) && units != Linear_Meter) {
		m_Error = wxString::Format(_("Projected units code %d is not metres (unsupported)"), (int)units);
		return false;
	}

	/* pixel -> easting/northing affine, taken from three image points so it
	   is right for both PixelIsArea and PixelIsPoint files, rotated or not */
	double x0 = 0, y0 = 0, x1 = 1, y1 = 0, x2 = 0, y2 = 1;
	if (!GTIFImageToPCS(gtif, &x0, &y0) || !GTIFImageToPCS(gtif, &x1, &y1) ||
	    !GTIFImageToPCS(gtif, &x2, &y2)) {
		m_Error = _("Unable to transform pixel/line to projected coordinates");
		return false;
	}

	Warp W;
	W.tm = p;
	W.e0 = x0;        W.n0 = y0;
	W.a  = x1 - x0;   W.c  = y1 - y0;
	W.b  = x2 - x0;   W.d  = y2 - y0;
	if (!W.Init(xsize, ysize)) {
		m_Error = _("Degenerate georeferencing (zero pixel size)");
		return false;
	}

	/* corner lat/lon for the XML and Goto, same slots as GTIFPrintCorners */
	memset(m_geoPolygon, 0, sizeof(m_geoPolygon));
	struct { double px, py; int idx; } pts[5] = {
		{ 0.0,        0.0,        0 },
		{ 0.0,        (double)ysize, 2 },
		{ (double)xsize, 0.0,        4 },
		{ (double)xsize, (double)ysize, 6 },
		{ xsize / 2.0, ysize / 2.0, 8 },
	};
	for (int i = 0; i < 5; i++) {
		double lat, lon;
		W.PixelToLatLon(pts[i].px, pts[i].py, lat, lon);
		m_geoPolygon[pts[i].idx]     = lon;
		m_geoPolygon[pts[i].idx + 1] = lat;
	}

	m_isTM = true;
	m_tm = p;
	m_aff[0] = W.e0; m_aff[1] = W.a; m_aff[2] = W.b;
	m_aff[3] = W.n0; m_aff[4] = W.c; m_aff[5] = W.d;
	wxLogMessage(wxString::Format(
		"ReadTransverse: cm=%.6f k0=%.6f fe=%.3f fn=%.3f e0=%.4f n0=%.4f a=%.8f b=%.8f c=%.8f d=%.8f",
		p.cm_deg, p.k0, p.fe, p.fn, W.e0, W.n0, W.a, W.b, W.c, W.d));
	return true;
}

bool PhotoLayer::ReadHeader(TIFF* m_Tiff, GTIF* m_gTiff, wxString filename) {

	int xsize = 0, ysize = 0;
	int	inv_flag = 0, dec_flag = 1;

	GTIFDefn defn;

	m_ModelType = -1;
	m_isTM = false;
	m_Error = wxEmptyString;

	bool gotW = TIFFGetField(m_Tiff, TIFFTAG_IMAGEWIDTH,  &xsize) != 0;
	bool gotH = TIFFGetField(m_Tiff, TIFFTAG_IMAGELENGTH, &ysize) != 0;

	if (!gotW || !gotH || xsize <= 0 || ysize <= 0) {
		wxLogMessage(wxString::Format(
			"ReadHeader: bad image dimensions gotW=%d gotH=%d xsize=%d ysize=%d for %s",
			gotW, gotH, xsize, ysize, filename));
		m_Error = _("Unable to read the image dimensions");
		return false;
	}

	imageWidthX = xsize;
	imageHeightY = ysize;

	/* Some GeoTIFFs (seen from SASPlanet) carry ModelPixelScale/ModelTiepoint
	   tags that are present but all-zero, or absent outright. Then
	   GTIFImageToPCS() cannot give a per-pixel transform, so fall back to a
	   sidecar .tfw/.tifw/.wld world file.
	   libtiff returns these custom tags as (count, pointer-to-array). */
	uint16_t scaleCount = 0, tieCount = 0, matCount = 0;
	double *scale = NULL, *tie = NULL, *mat = NULL;
	TIFFGetField(m_Tiff, TIFFTAG_GEOPIXELSCALE, &scaleCount, &scale);
	TIFFGetField(m_Tiff, TIFFTAG_GEOTIEPOINTS, &tieCount, &tie);
	TIFFGetField(m_Tiff, TIFFTAG_GEOTRANSMATRIX, &matCount, &mat);

	bool haveTransform = (matCount >= 16 && mat) ||
		(scaleCount >= 2 && scale && tieCount >= 6 && tie &&
		 !(scale[0] == 0.0 && scale[1] == 0.0));

	if (!haveTransform) {
		double A, Bp, Cp, D, E, F;
		if (ReadWorldFile(filename, A, Bp, Cp, D, E, F)) {
			/* No rotation term supported here (B/D assumed 0, i.e. a plain
			   axis-aligned world file). (Cp,F) is the CENTER of the
			   top-left pixel, so corners are half a pixel further out. */
			double lon_ul = Cp - A / 2.0,               lat_ul = F - E / 2.0;
			double lon_lr = Cp + A * (xsize - 0.5),     lat_lr = F + E * (ysize - 0.5);

			memset(m_geoPolygon, 0, sizeof(m_geoPolygon));
			m_geoPolygon[0] = lon_ul; m_geoPolygon[1] = lat_ul;   /* Upper Left  */
			m_geoPolygon[6] = lon_lr; m_geoPolygon[7] = lat_lr;   /* Lower Right */
			m_geoPolygon[8] = (lon_ul + lon_lr) / 2.0;            /* Center      */
			m_geoPolygon[9] = (lat_ul + lat_lr) / 2.0;

			/* World-file georeferencing as used here is always plain
			   lat/lon degrees. A world file in projected metres is not
			   handled (its CRS is unknowable from the .tfw alone). */
			m_ModelType = ModelTypeGeographic;
			return true;
		}
		wxLogMessage("ReadHeader: no usable embedded GeoTIFF tags and no world file found for " + filename);
		m_Error = _("No usable georeferencing (no GeoTIFF tags and no .tfw world file)");
		return false;
	}

	if (!m_gTiff || !GTIFGetDefn(m_gTiff, &defn)) {
		m_Error = _("Unable to read the GeoTIFF keys");
		return false;
	}

	m_ModelType = defn.Model;
	wxLogMessage(wxString::Format("ReadHeader: Model=%d PCS=%d GCS=%d for %s",
		(int)defn.Model, (int)defn.PCS, (int)defn.GCS, filename));

	if (defn.Model == ModelTypeProjected) {
		int pcs = defn.PCS;
		PhotoLayerTM::Params probe;
		bool isUTM = PhotoLayerTM::UTMFromEPSG(pcs, probe);

		bool isTMuser = false, isMercUser = false;
		if (pcs == 32767) {
			unsigned short ct = 0;
			GTIFKeyGet(m_gTiff, ProjCoordTransGeoKey, &ct, 0, 1);
			isTMuser = (ct == CT_TransverseMercator);
			isMercUser = (ct == CT_Mercator);
		}

		if (isUTM || isTMuser)
			return ReadTransverse(m_gTiff, &defn, xsize, ysize);

		/* Mercator family, handled by the closed-form inverse in
		   GTIFReportACorner(): anything else is refused rather than drawn in
		   the wrong place. */
		bool mercOK = (pcs == 3857 || pcs == 3785 || pcs == 900913 ||
		               pcs == 3395 || isMercUser);
		if (!mercOK) {
			m_Error = wxString::Format(
				_("Unsupported projected CRS EPSG:%d. Supported: lat/lon, Web/World Mercator, and UTM (WGS84/ETRS89/NAD83)."), pcs);
			wxLogMessage("ReadHeader: " + m_Error);
			return false;
		}
	}

	GTIFPrintCorners(m_gTiff, &defn, xsize, ysize, inv_flag, dec_flag);
	return true;
}

void PhotoLayer::OpenImage(wxString filename, wxString station, wxString area, wxString contents)
{
	TIFF *tif = (TIFF*)0;  /* TIFF-level descriptor */
	GTIF *gtif = (GTIF*)0; /* GeoKey-level descriptor */


	tif = XTIFFOpen(filename, "r");
	if (!tif) {
		wxMessageDialog mdlg(this, _("Failed to open file: ") + filename,
			_("PhotoLayer"), wxOK | wxICON_ERROR);
		mdlg.ShowModal();
		return;
	}
	gtif = GTIFNew(tif);

	bool headerOK = ReadHeader(tif, gtif, filename);

	/* FIX: gtif was never freed (GTIFFree missing). On some platforms an
	   unreleased GeoTIFF handle can leave the underlying file effectively
	   still open, which caused the immediately-following wxImage::LoadFile()
	   to occasionally fail on the very first open of a file. */
	if (gtif)
		GTIFFree(gtif);
	XTIFFClose(tif);

	if (!headerOK) {
		/* never fall through with stale corner data from a previous file */
		wxMessageDialog mdlg(this, _("Cannot place this image: ") + m_Error + _T("\n") + filename,
			_("PhotoLayer"), wxOK | wxICON_ERROR);
		mdlg.ShowModal();
		return;
	}

	int transparency = m_sTransparency->GetValue();
	int whitetransparency = m_sWhiteTransparency->GetValue();
	bool invert = m_cInvert->GetValue();

	/* FIX (was: m_BuiltinCoords.Clear()): that cleared the ENTIRE list on
	   every open, wiping out every other already-loaded file's coordinate
	   entry too - so only the most recently opened file ever ended up in
	   m_BuiltinCoords, and SaveTIFCoordinatesToXml() then wrote out an XML
	   file containing just that one entry. The actual problem being solved
	   was narrower: prevent a STALE entry for the SAME filename (e.g. left
	   over from an earlier, differently-computed open of this exact file)
	   from shadowing the freshly computed one in the match loop below.
	   Remove only entries matching this filename - same pattern already
	   used by UpdateDataSet() - and leave every other file's entry alone. */
	{
		PhotoLayerImageCoordinateList kept;
		for (unsigned int i = 0; i < m_BuiltinCoords.GetCount(); i++) {
			if (m_BuiltinCoords[i]->name != filename)
				kept.Append(m_BuiltinCoords[i]);
			else
				delete m_BuiltinCoords[i];   /* free the stale entry being replaced */
		}
		m_BuiltinCoords = kept;
	}

	LoadCoordinatesFromTIF(m_BuiltinCoords, filename);
	SaveTIFCoordinatesToXml(m_BuiltinCoords, _T("PhotoLayerDataSets.xml"));

	PhotoLayerImageCoordinateList BuiltinCoordList;
	wxImage wimg;

	if (!wimg.LoadFile(filename)) {

		{
			wxMessageDialog mdlg(this, _("Failed to load input file: ") + filename,
				_("PhotoLayer"), wxOK | wxICON_ERROR);
			mdlg.ShowModal();
			UpdateDataSet(filename);
			return;
		}
	}

	PhotoLayerImage *img = new PhotoLayerImage(wimg, transparency, whitetransparency, invert);
	wxString name = filename;  // _T("TIF");

	/* FIX: MakeMappedImage()'s return value used to be checked only via a
	   'goto wizarddone' that unconditionally fell through to the same label
	   whether or not it fired - so a failed mapping (NaN corners, negative
	   or zero dimensions, oversize image) still resulted in the broken
	   image being added to m_Faxes/m_lFaxes and then Goto() being called on
	   its degenerate m_mappedimg. Now a failed mapping is caught, logged,
	   and the image is discarded instead of being shown. */
	bool mapped = false;
	for (unsigned int i = 0; i < m_BuiltinCoords.GetCount(); i++)
		if (name == m_BuiltinCoords[i]->name) {
			img->m_Coords = m_BuiltinCoords[i];
			img->MakePhasedImage();
			if (img->MakeMappedImage(this)) {
				mapped = true;
				break;
			}
		}

	if (!mapped) {
		wxLogMessage("OpenImage: MakeMappedImage failed for " + filename);
		delete img;
		UpdateDataSet(filename);
		return;
	}

	name = station.size() && contents.size() ? (station + _T(" - ") + contents) : filename;
	int selection = m_lFaxes->Append(name);
	m_Faxes.push_back(img);

	m_lFaxes->DeselectAll();
	/* FIX: was SetSelection(selection). wxListBox::SetSelection() is only
	   reliable for single-selection listboxes; m_lFaxes is wxLB_EXTENDED
	   (multi-select), for which wx documents Select() as the correct call.
	   SetSelection() on a multi-select box could leave IsSelected() out of
	   sync with the visibly-highlighted row, which fed into RenderOverlay's
	   IsSelected() check and could prevent the just-opened image from
	   rendering until the row was clicked manually. */
	m_lFaxes->Select(selection);

	Goto(selection);

	RequestRefresh(m_parent);
	UpdateMenuStates();

	if (BuiltinCoordList.GetCount())
		m_BuiltinCoords.Append(BuiltinCoordList[0]);
}


void PhotoLayer::Goto(int selection)
{
    PhotoLayerImage &image = *m_Faxes[selection];
    double lat0 = image.m_Coords->lat(0), lat1 = image.m_Coords->lat(image.m_mappedimg.GetHeight());
    double lon0 = image.m_Coords->lon(0), lon1 = image.m_Coords->lon(image.m_mappedimg.GetWidth());
    if(lon0 - lon1 > 180)
        lon1 += 360;

    double distance;
    DistanceBearingMercator_Plugin(lat0, lon0, lat1, lon1, NULL, &distance);

    if(!isnan(distance))
        JumpToPosition((lat0 + lat1) / 2, (lon0 + lon1) / 2, .5/distance);
}

void PhotoLayer::OnOpen( wxCommandEvent& event )
{
	wxFileDialog openDialog
		(this, _("Open PhotoLayer Input File"),
		m_PhotoLayer_pi.m_path, wxT(""),
		_("\
		  GeoTiff files|*.tif;*.TIF;*.tiff;*.TIFF|\
All files (*.*)|*.*" ), wxFD_OPEN);

    if( openDialog.ShowModal() == wxID_OK ) {
        wxString filename = openDialog.GetPath();
        m_PhotoLayer_pi.m_path = openDialog.GetDirectory();
        OpenImage(filename);
    }

    /* FIX: reclaim focus for the main canvas after the modal file dialog
       closes. Without this, the canvas could be left without proper
       input focus/activation until the user clicked it directly. */
    m_PhotoLayer_pi.m_parent_window->SetFocus();
    RequestRefresh(m_PhotoLayer_pi.m_parent_window);
}

void PhotoLayer::OnSaveAs( wxCommandEvent& event )
{
    for(int selection = 0; selection < (int)m_Faxes.size(); selection++) {
        if(!m_lFaxes->IsSelected(selection))
            continue;

        PhotoLayerImage &image = *m_Faxes[selection];

        wxFileDialog saveDialog
            ( this, _( "Save PhotoLayer To Image" ),
              m_PhotoLayer_pi.m_export_path, image.m_Coords->name + _T(".png"),
              _ ( "\
Image Files|*.BMP;*.bmp|*.PNG;*.png|*.TIFF;*.tiff\
All files (*.*)|*.*" ), wxFD_SAVE);

        if( saveDialog.ShowModal() == wxID_OK ) {
            wxString filename = saveDialog.GetPath();
            m_PhotoLayer_pi.m_export_path = saveDialog.GetDirectory();

            if(!image.m_mappedimg.SaveFile(filename)) {
                wxMessageDialog mdlg(this, _("Failed to save file: ") + filename,
                                 _("PhotoLayer"), wxOK | wxICON_ERROR);
                mdlg.ShowModal();
            }
        }
    }
}

void PhotoLayer::OnGoto( wxCommandEvent& event )
{
    /* take first selection.. we could instead average or something... */
    int selection;
    for(selection = 0; !m_lFaxes->IsSelected(selection); selection++)
        if(selection == (int)m_Faxes.size())
            return;

    Goto(selection);
}

void PhotoLayer::OnExport( wxCommandEvent& event )
{
    for(int selection = 0; selection < (int)m_Faxes.size(); selection++) {
        if(!m_lFaxes->IsSelected(selection))
            continue;

        PhotoLayerImage &image = *m_Faxes[selection];

        wxFileDialog saveDialog
            ( this, _( "Save PhotoLayer To KAP" ),
              m_PhotoLayer_pi.m_export_path, image.m_Coords->name + _T(".kap"),
              _ ( "\
KAP Files|*.KAP;*.kap|\
All files (*.*)|*.*" ), wxFD_SAVE);

        if( saveDialog.ShowModal() == wxID_OK ) {
            wxString filename = saveDialog.GetPath();
            m_PhotoLayer_pi.m_export_path = saveDialog.GetDirectory();

            wximgtokap(image, m_PhotoLayer_pi.m_iExportColors,
                       m_PhotoLayer_pi.m_bExportDepthMeters ? METERS : FATHOMS,
                       m_PhotoLayer_pi.m_sExportSoundingDatum.mb_str(), filename.mb_str());
        } else
            break;
    }
}

void PhotoLayer::OnDelete( wxCommandEvent& event )
{
	wxString filename;

	for(int selection = 0; selection < (int)m_Faxes.size(); selection++) {
		if (m_lFaxes->IsSelected(selection)) {

			filename = m_Faxes[selection]->m_Coords->name;

			wxMessageDialog *dial = new wxMessageDialog(NULL,
				wxT("YES to DELETE the file\n\nNO to remove from the list\n... but NOT delete the file"), wxT("Question"),
				wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);


			if (dial->ShowModal() == wxID_YES) {
				wxRemoveFile(filename);
				UpdateDataSet(filename);

			}
			else {

				wxMessageBox(_("Image will be removed from the list\nbut NOT deleted"), _("Remove from list"));
				UpdateDataSet(filename);
			}

			delete m_Faxes[selection];
			m_Faxes.erase(m_Faxes.begin() + selection);

			m_lFaxes->Delete(selection);
			UpdateMenuStates();

			RequestRefresh(m_parent);
			selection--;
		}
    }
}

void PhotoLayer::UpdateDataSet(wxString filename){

	PhotoLayerImageCoordinateList BuiltinCoordList;

	for (unsigned int i = 0; i < m_BuiltinCoords.GetCount(); i++){
		if (filename != m_BuiltinCoords[i]->name) {
			BuiltinCoordList.Append(m_BuiltinCoords[i]);
		}
	}

	m_BuiltinCoords = BuiltinCoordList;

	wxString xmlFileName = _T("PhotoLayerDataSets.xml");
	wxString path = PhotoLayer_pi::StandardPath();
	wxString s = wxFileName::GetPathSeparator();
	wxString datasetsfile = path + s + xmlFileName;   /* FIX: missing path separator meant this built
	                                                      a filename like "...\datasetsPhotoLayerDataSets.xml",
	                                                      so wxRemoveFile() below always failed. */

	wxRemoveFile(datasetsfile);
	SaveTIFCoordinatesToXml(m_BuiltinCoords, xmlFileName);
}

void PhotoLayer::TransparencyChanged( wxScrollEvent& event )
{
    bool update = false;
    for(int selection = 0; selection < (int)m_Faxes.size(); selection++) {
        if(!m_lFaxes->IsSelected(selection))
            continue;

        PhotoLayerImage &image = *m_Faxes[selection];
        image.m_iTransparency = event.GetPosition();
        update = true;
    }

    if(update)
        RequestRefresh( m_parent );
}

void PhotoLayer::WhiteTransparencyChanged( wxScrollEvent& event )
{
    bool update = false;
    for(int selection = 0; selection < (int)m_Faxes.size(); selection++) {
        if(!m_lFaxes->IsSelected(selection))
            continue;

        PhotoLayerImage &image = *m_Faxes[selection];
        image.m_iWhiteTransparency = event.GetPosition();
        image.FreeData();
        update = true;
    }

    if(update)
        RequestRefresh( m_parent );
}

void PhotoLayer::OnInvert( wxCommandEvent& event )
{
    bool update = false;
    for(int selection = 0; selection < (int)m_Faxes.size(); selection++) {
        if(!m_lFaxes->IsSelected(selection))
            continue;

        PhotoLayerImage &image = *m_Faxes[selection];
        image.m_bInvert = event.IsChecked();
        image.FreeData();
        update = true;
    }

    if(update)
        RequestRefresh( m_parent );
}

void PhotoLayer::OnAbout( wxCommandEvent& event )
{
    AboutDialog dlg(this);
    dlg.ShowModal();
}

bool PhotoLayer::Show( bool show )
{

    return PhotoLayerBase::Show(show);
}

void PhotoLayer::UpdateMenuStates()
{
    wxArrayInt Selections;
    bool e = m_lFaxes->GetSelections(Selections) != 0;
    m_mSaveAs->Enable(e);
    m_mGoto->Enable(e);
	m_mExport->Enable(e);
    m_mDelete->Enable(e);
    EnableDisplayControls(e);
}
