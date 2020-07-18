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

#include <vector>

#include "PhotoLayerUI.h"

#if not defined WIN32
#define __int64 long int
#include <tiffio.h>
#include <xtiffio.h>
#include <geotiffio.h>
#endif

	#include "geotiffio.h"
	#include "xtiffio.h"
	#include "geo_normalize.h"
	#include "geo_simpletags.h"
	#include "geovalues.h"
	#include "tiffio.h"
	#include "cpl_serv.h"


#include <wx/dynarray.h>
#include <ctype.h>
#include <string>
#include <algorithm>
#include <cstring>
#include <wchar.h>
#include <bitset>

enum { VERSION = 0, MAJOR, MINOR };

extern const char * box_xpm[];
extern const char * check_xpm[];

using namespace std;

class PhotoLayer_pi;

class PhotoLayer : public PhotoLayerBase
{
public:
    PhotoLayer( PhotoLayer_pi &_PhotoLayer_pi, wxWindow* parent);
    ~PhotoLayer();

	void OnClose(wxCloseEvent& event);
    void EnableDisplayControls(bool enable);
	void ShowSavedImages();
    void OnFaxes( wxCommandEvent& event );
    void TransparencyChanged( wxScrollEvent& event );
    void WhiteTransparencyChanged( wxScrollEvent& event );
    void OnInvert( wxCommandEvent& event );
    void OnOpen( wxCommandEvent& event );
    void OnSaveAs( wxCommandEvent& event );
    void OnGoto( wxCommandEvent& event );
    void OnDelete( wxCommandEvent& event );
	void UpdateDataSet(wxString filename);
    void OnExport( wxCommandEvent& event );
    void OnPreferences( wxCommandEvent& event ) { m_PhotoLayer_pi.ShowPreferencesDialog( this ); }
    void OnAbout( wxCommandEvent& event );

    bool Show( bool show = true );
	bool ReadHeader(TIFF* m_Tiff, GTIF* m_gTiff);
    void OpenImage(wxString filename, wxString station=_T(""), wxString area=_T(""), wxString contents=_T(""));
    void Goto(int selection);

    void UpdateMenuStates();

    PhotoLayerImageCoordinateList m_BuiltinCoords, m_UserCoords;

    std::vector<PhotoLayerImage*>m_Faxes;

    PhotoLayer_pi &m_PhotoLayer_pi;

	int m_Compression = 0;
	wxString m_Copyright;
	wxString m_Description;
	wxString m_Error;
	int m_Height = 0;
	int m_JpegColorMode = 0;
	double m_geoPolygon[10];
	GTIF* m_Tiff;
	int m_TileLength = 0;
	int m_TileWidth = 0;
	int m_Width= 0;
    


protected:
	

private:
	void LoadCoordinatesFromTIF(PhotoLayerImageCoordinateList &coords, wxString filename);
	void LoadTIFCoordinatesFromXml(PhotoLayerImageCoordinateList &coords, wxString coordinatesets);
	void SaveTIFCoordinatesToXml(PhotoLayerImageCoordinateList &coords, wxString filename);
	int GTIFReportACorner(GTIF *gtif, GTIFDefn *defn, 
		const char * corner_name,
		double x, double y, int inv_flag, int dec_flag);
	
	void GTIFPrintCorners(GTIF *gtif, GTIFDefn *defn, 
		int xsize, int ysize, int inv_flag, int dec_flag);
	
	int cornerNum;
	int imageWidthX, imageHeightY;
	
};

