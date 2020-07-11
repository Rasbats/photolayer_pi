/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  PhotoLayer Plugin
 * Author:   Mike Rossiter, based on WeatherFax by Sean D'Epagnier
 *
 ***************************************************************************
 *   Copyright (C) 2017 by Mike Rossiter                                 *
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
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include "PhotoLayer_pi.h"
#include "PhotoLayerImage.h"
#include "PhotoLayer.h"
#include "icons.h"

#include "version.h"
#include "wxWTranslateCatalog.h"

// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr)
{
    return new PhotoLayer_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p)
{
    delete p;
}

//---------------------------------------------------------------------------------------------------------
//
//    PhotoLayer PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------

PhotoLayer_pi::PhotoLayer_pi(void *ppimgr)
    :opencpn_plugin_116(ppimgr)
{
    // Create the PlugIn icons
    initialize_images();
	wxFileName fn;
	wxString tmp_path;

	tmp_path = GetPluginDataDir("photolayer_pi");
	fn.SetPath(tmp_path);
	fn.AppendDir(_T("data"));
	fn.SetFullName("photolayer_panel_icon.png");

	wxString shareLocn = fn.GetFullPath();

	wxImage panelIcon(shareLocn);
	if (panelIcon.IsOk())
		m_panelBitmap = wxBitmap(panelIcon);
	else
		wxLogMessage(_T("    PhotoLayer panel icon NOT loaded"));
}

PhotoLayer_pi::~PhotoLayer_pi(void)
{

}

//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

int PhotoLayer_pi::Init(void)
{
    AddLocaleCatalog(PLUGIN_CATALOG_NAME);
    
    m_pPhotoLayer = NULL;

	m_bShowPhoto = false;

	
	m_leftclick_tool_id = InsertPlugInToolSVG(_T(""), _svg_photolayer, _svg_photolayer, _svg_photolayer_toggled, 
			wxITEM_CHECK, _("PhotoLayer"), _T(""), NULL, PHOTOLAYER_TOOL_POSITION, 0, this);



    return (WANTS_OVERLAY_CALLBACK |
            WANTS_OPENGL_OVERLAY_CALLBACK |
            WANTS_TOOLBAR_CALLBACK    |
            INSTALLS_TOOLBAR_TOOL     |
            WANTS_PREFERENCES         |
            WANTS_CONFIG
        );
}

bool PhotoLayer_pi::DeInit(void)
{
    SaveConfig();
    //    Record the dialog position
	if (NULL != m_pPhotoLayer)
	{
		//Capture dialog position
		wxPoint p = m_pPhotoLayer->GetPosition();
		SetPhotoLayerDialogX(p.x);
		SetPhotoLayerDialogY(p.y);
        m_pPhotoLayer->Close();
        delete m_pPhotoLayer;
        m_pPhotoLayer = NULL;
    }

    RemovePlugInTool(m_leftclick_tool_id);
    return true;
}

int PhotoLayer_pi::GetAPIVersionMajor()
{
    return OCPN_API_VERSION_MAJOR;
}

int PhotoLayer_pi::GetAPIVersionMinor()
{
    return OCPN_API_VERSION_MINOR;
}

int PhotoLayer_pi::GetPlugInVersionMajor()
{
    return PLUGIN_VERSION_MAJOR;
}

int PhotoLayer_pi::GetPlugInVersionMinor()
{
    return PLUGIN_VERSION_MINOR;
}

wxBitmap *PhotoLayer_pi::GetPlugInBitmap()
{
    return &m_panelBitmap;
}

wxString PhotoLayer_pi::GetCommonName()
{
    return PLUGIN_COMMON_NAME;
}

wxString PhotoLayer_pi::GetShortDescription()
{
    return _("PhotoLayer PlugIn for OpenCPN");
}

wxString PhotoLayer_pi::GetLongDescription()
{
    return _("PhotoLayer PlugIn for OpenCPN\n\
A viewer for GeoTiff image files.\n\
Builtin record of images that have been loade.\n\
\n\
");
}

int PhotoLayer_pi::GetToolbarToolCount(void)
{
    return 1;
}

void PhotoLayer_pi::SetColorScheme(PI_ColorScheme cs)
{
    if (NULL == m_pPhotoLayer)
        return;
    DimeWindow(m_pPhotoLayer);
}

void PhotoLayer_pi::RearrangeWindow()
{
    if (NULL == m_pPhotoLayer)
        return;

    SetColorScheme(PI_ColorScheme());

    m_pPhotoLayer->Fit();
}

void PhotoLayer_pi::OnToolbarToolCallback(int id)
{
    if(!m_pPhotoLayer) {
        // Set some default private member parameters
        m_PhotoLayer_dialog_x = 0;
        m_PhotoLayer_dialog_y = 0;

        ::wxDisplaySize(&m_display_width, &m_display_height);

        //    Get a pointer to the opencpn display canvas, to use as a parent for the POI Manager dialog
        m_parent_window = GetOCPNCanvasWindow();

        //    And load the configuration items
		LoadConfig();

        m_pPhotoLayer = new PhotoLayer(*this, m_parent_window);
        m_pPhotoLayer->Move(wxPoint(m_PhotoLayer_dialog_x, m_PhotoLayer_dialog_y));

    }

	//Toggle GRIB overlay display
	m_bShowPhoto = !m_bShowPhoto;

	//    Toggle dialog?
	if (m_bShowPhoto) {
		m_pPhotoLayer->Show();
		RequestRefresh(m_parent_window); // refresh main window
	}
	else {
		m_pPhotoLayer->Hide();
	}


    RearrangeWindow();

    wxPoint p = m_pPhotoLayer->GetPosition();
    m_pPhotoLayer->Move(0,0);        // workaround for gtk autocentre dialog behavior
    m_pPhotoLayer->Move(p);
}



bool PhotoLayer_pi::RenderOverlay(wxDC &dc, PlugIn_ViewPort *vp)
{
    if(!m_pPhotoLayer || !m_pPhotoLayer->IsShown())
        return true;

    for(unsigned int i=0; i<m_pPhotoLayer->m_lFaxes->GetCount(); i++)
        if(m_pPhotoLayer->m_lFaxes->IsSelected(i))
            m_pPhotoLayer->m_Faxes[i]->RenderImage(dc, vp);

    return true;
}

void PhotoLayer_pi::OnDialogClose()
{
	m_bShowPhoto = false;
	SetToolbarItemState(m_leftclick_tool_id, m_bShowPhoto);
	m_pPhotoLayer->Hide();
	RequestRefresh(m_parent_window); // refresh main window
}

bool PhotoLayer_pi::RenderGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp)
{
    if(!m_pPhotoLayer || !m_pPhotoLayer->IsShown())
        return true;

    for(unsigned int i=0; i<m_pPhotoLayer->m_lFaxes->GetCount(); i++)
        if(m_pPhotoLayer->m_lFaxes->IsSelected(i))
            m_pPhotoLayer->m_Faxes[i]->RenderImageGL(vp);

    return true;
}

wxString PhotoLayer_pi::StandardPath()
{
   
	wxString stdPath(*GetpPrivateApplicationDataLocation());
	wxString s = wxFileName::GetPathSeparator();

	stdPath += s + _T("plugins") + s + _T("photolayer_pi");
	if (!wxDirExists(stdPath))
		wxMkdir(stdPath);
	stdPath += s + _T("data");
	if (!wxDirExists(stdPath))
		wxMkdir(stdPath);
	
    return stdPath;
}

bool PhotoLayer_pi::LoadConfig(void)
{
    wxFileConfig *pConf = GetOCPNConfigObject();

    if(!pConf)
        return false;

	wxString m_export_colors;

	pConf->SetPath(_T("/Settings/PhotoLayer"));
	pConf->Read(_T("Path"), &m_path, _T(""));
	pConf->Read(_T("ExportPath"), &m_export_path, _T(""));
	pConf->Read(_T("ExportColors"), &m_export_colors, _T(""));

	m_iExportColors = wxAtoi(m_export_colors);

	m_PhotoLayer_dialog_x = pConf->Read(_T("DialogPosX"), 20L);
	m_PhotoLayer_dialog_y = pConf->Read(_T("DialogPosY"), 20L);

    return true;
}

bool PhotoLayer_pi::SaveConfig(void)
{
    wxFileConfig *pConf = GetOCPNConfigObject();

    if(!pConf)
        return false;

	pConf->SetPath(_T("/Settings/PhotoLayer"));
	pConf->Write(_T("Path"), m_path);
	pConf->Write(_T("ExportPath"), m_export_path);
	pConf->Write(_T("ExportColors"), wxString::Format(_T("%i"), m_iExportColors));
	if (m_pPhotoLayer) {
		wxPoint p = m_pPhotoLayer->GetPosition();
		pConf->Write(_T("DialogPosX"), p.x);
		pConf->Write(_T("DialogPosY"), p.y);
	}


    return true;
}

void PhotoLayer_pi::SetPositionFixEx(PlugIn_Position_Fix_Ex &pfix)
{
    m_lastfix = pfix;
}

void PhotoLayer_pi::ShowPreferencesDialog( wxWindow* parent )
{
    PhotoLayerPrefsDialog *dialog =
        new PhotoLayerPrefsDialog( parent, wxID_ANY, _("PhotoLayer Preferences"),
                                   wxPoint( m_PhotoLayer_dialog_x, m_PhotoLayer_dialog_y),
                                   wxDefaultSize, wxDEFAULT_DIALOG_STYLE );

    dialog->m_sExportColors->SetValue(m_iExportColors);
    dialog->m_rbExportDepthMeters->SetValue(m_bExportDepthMeters);
    dialog->m_tExportSoundingDatum->SetValue(m_sExportSoundingDatum);

    dialog->Fit();
    wxColour cl;
    GetGlobalColor(_T("DILG1"), &cl);
    dialog->SetBackgroundColour(cl);

    if(dialog->ShowModal() == wxID_OK)
    {
        m_iExportColors = dialog->m_sExportColors->GetValue();
        m_bExportDepthMeters = dialog->m_rbExportDepthMeters->GetValue();
        m_sExportSoundingDatum = dialog->m_tExportSoundingDatum->GetValue();

        SaveConfig();
    }
    delete dialog;
}

