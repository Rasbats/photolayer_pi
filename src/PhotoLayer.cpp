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
 */

#include <list>
#include "tinyxml/tinyxml.h"
#include "PhotoLayer_pi.h"
#include "PhotoLayerImage.h"
#include "PhotoLayer.h"
#include "AboutDialog.h"
#include "icons.h"
#include "georef.h"
#include "wximgkap.h"

/* XPM */
static char *photolayer_xpm[] = {
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
*/

int PhotoLayer::GTIFReportACorner(GTIF *gtif, GTIFDefn *defn,
	const char * corner_name,
	double x, double y, int inv_flag, int dec_flag)

{
	double	x_saved, y_saved;

	/* Try to transform the coordinate into PCS space */
	if (!GTIFImageToPCS(gtif, &x, &y))
		return FALSE;

	x_saved = x;
	y_saved = y;

	if (defn->Model == ModelTypeGeographic)
	{
		if (dec_flag)
		{
			m_geoPolygon[cornerNum] = x;
			cornerNum++;
			m_geoPolygon[cornerNum] = y;

			return true;

		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return false;
}

void PhotoLayer::GTIFPrintCorners(GTIF *gtif, GTIFDefn *defn,
	int xsize, int ysize, int inv_flag, int dec_flag)
{
	printf("\nCorner Coordinates:\n");
	cornerNum = 0;

	if (!GTIFReportACorner(gtif, defn,
		"Upper Left", 0.0, 0.0, inv_flag, dec_flag))
	{
		wxMessageBox(_(" ... unable to transform points between pixel/line and PCS space\n"));
		return;
	}
	cornerNum++;
	GTIFReportACorner(gtif, defn,  "Lower Left", 0.0, ysize,
		inv_flag, dec_flag);
	cornerNum++;
	GTIFReportACorner(gtif, defn,  "Upper Right", xsize, 0.0,
		inv_flag, dec_flag);
	cornerNum++;
	GTIFReportACorner(gtif, defn,  "Lower Right", xsize, ysize,
		inv_flag, dec_flag);
	cornerNum++;
	GTIFReportACorner(gtif, defn,  "Center", xsize / 2.0, ysize / 2.0,
		inv_flag, dec_flag);
	cornerNum++;
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

void PhotoLayer::LoadCoordinatesFromTIF(PhotoLayerImageCoordinateList &coords, wxString filename)
{
	wxString name = filename;

	PhotoLayerImageCoordinates* coord = new PhotoLayerImageCoordinates(name);
	coord->name = filename;
	coord->p1.x = 0;
	coord->p1.y = 0;
	coord->lat1 = m_geoPolygon[1];
	coord->lon1 = m_geoPolygon[0];

	coord->p2.x = imageWidthX;  //3265
	coord->p2.y = imageHeightY;  //3776

	coord->lat2 = m_geoPolygon[7];
	coord->lon2 = m_geoPolygon[6];

	coord->CenterLat = m_geoPolygon[9];
	coord->CenterLon = m_geoPolygon[8];

	coord->rotation = PhotoLayerImageCoordinates::NONE;
	coord->mapping = PhotoLayerImageCoordinates::MERCATOR;

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
		c->SetAttribute("Lat1", wxString::Format(_T("%.5f"), coords[i]->lat1).mb_str());
		c->SetAttribute("Lon1", wxString::Format(_T("%.5f"), coords[i]->lon1).mb_str());

		c->SetAttribute("X2", wxString::Format(_T("%d"), coords[i]->p2.x).mb_str());
		c->SetAttribute("Y2", wxString::Format(_T("%d"), coords[i]->p2.y).mb_str());
		c->SetAttribute("Lat2", wxString::Format(_T("%.5f"), coords[i]->lat2).mb_str());
		c->SetAttribute("Lon2", wxString::Format(_T("%.5f"), coords[i]->lon2).mb_str());
		c->SetAttribute("CenterLat", wxString::Format(_T("%.5f"), coords[i]->CenterLat).mb_str());
		c->SetAttribute("CenterLon", wxString::Format(_T("%.5f"), coords[i]->CenterLon).mb_str());

		root->LinkEndChild(c);
	}

	wxString layer_path = PhotoLayer_pi::StandardPath();

	if (!doc.SaveFile((layer_path +  filename).mb_str()))
		wxLogMessage(_("PhotoLayer") + wxString(_T(": ")) + _("Failed to save xml file: ") + filename);
}

void PhotoLayer::LoadTIFCoordinatesFromXml(PhotoLayerImageCoordinateList &coords, wxString coordinatesets)
{
    TiXmlDocument doc;
	wxString name;
    wxString error;
    wxString coordinatesets_path = PhotoLayer_pi::StandardPath();

    if(!doc.LoadFile((coordinatesets_path + coordinatesets).mb_str()))
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
	wxString blank_name = *GetpSharedDataLocation()
		+ _T("plugins/PhotoLayer_pi/data/blank.ico");

	wxIcon icon(blank_name, wxBITMAP_TYPE_ICO);
	SetIcon(icon);

	SetIcon(icon);
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

		if (!wimg.LoadFile(name)) {
			{
				wxMessageDialog mdlg(this, _("Failed to load input file: ") + name,
					_("PhotoLayer"), wxOK | wxICON_ERROR);
				mdlg.ShowModal();
				return;
			}
		}
		PhotoLayerImage *img = new PhotoLayerImage(wimg, transparency, whitetransparency, invert);
		img->m_Coords = m_BuiltinCoords[i];
		img->MakePhasedImage();
		if (img->MakeMappedImage(this)){
			m_lFaxes->Append(name);
			m_Faxes.push_back(img);

		}
	}
	m_lFaxes->SetSelection(0);
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


bool PhotoLayer::ReadHeader(TIFF* m_Tiff, GTIF* m_gTiff) {

	int		xsize, ysize;
	int		inv_flag = 0, dec_flag = 1;

	GTIFDefn defn;

	if (GTIFGetDefn(m_gTiff, &defn))
	{
		TIFFGetField(m_Tiff, TIFFTAG_IMAGEWIDTH, &xsize);
		TIFFGetField(m_Tiff, TIFFTAG_IMAGELENGTH, &ysize);

		imageWidthX = xsize;
	    imageHeightY = ysize;

		GTIFPrintCorners(m_gTiff, &defn, xsize, ysize, inv_flag, dec_flag);
		return true;
	}
	else {
		return false;
	}
	return false;
}

void PhotoLayer::OpenImage(wxString filename, wxString station, wxString area, wxString contents)
{
	TIFF *tif = (TIFF*)0;  /* TIFF-level descriptor */
	GTIF *gtif = (GTIF*)0; /* GeoKey-level descriptor */


	tif = XTIFFOpen(filename, "r");
	gtif = GTIFNew(tif);


	ReadHeader(tif, gtif);


	XTIFFClose(tif);

	int transparency = m_sTransparency->GetValue();
	int whitetransparency = m_sWhiteTransparency->GetValue();
	bool invert = m_cInvert->GetValue();
	//m_BuiltinCoords.Clear();
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

	for (unsigned int i = 0; i < m_BuiltinCoords.GetCount(); i++)
		if (name == m_BuiltinCoords[i]->name) {
			img->m_Coords = m_BuiltinCoords[i];
			img->MakePhasedImage();
			if (img->MakeMappedImage(this))
				goto wizarddone;
		}

wizarddone:
	name = station.size() && contents.size() ? (station + _T(" - ") + contents) : filename;
	int selection = m_lFaxes->Append(name);
	m_Faxes.push_back(img);

	m_lFaxes->DeselectAll();
	m_lFaxes->SetSelection(selection);
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
#if 0 // for opencpn 3.3 and later
    DistanceBearingMercator_Plugin(lat0, lon0, lat1, lon1, NULL, &distance);
#else
    WFDistanceBearingMercator(lat0, lon0, lat1, lon1, NULL, &distance);
#endif
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
	wxString datasetsfile = path + xmlFileName;

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



