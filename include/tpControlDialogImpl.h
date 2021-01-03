/***************************************************************************
* Project:  OpenCPN
* Purpose:  Test Plugin Control Dialog
* Author:   Jon Gough
*
* Project:  OpenCPN
*
***************************************************************************
*   Copyright (C) 2013 by David S. Register                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
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
**************************************************************************/

#ifndef __TPCONTROLDIALOGIMPL_H__
#define __TPCONTROLDIALOGIMPL_H__

/**
 * @file
 * Subclass of tpControlDef, which is generated by wxFormBuilder.
 */

#include "tpControlDialogDef.h"
#include "ODAPI.h"

//    Constants for buttons

// forward class definition
class wxFontDialog;

///////////////////////////////////////////////////////////////////////////////
/// Class tpControlDialogImpl
///////////////////////////////////////////////////////////////////////////////
class tpControlDialogImpl : public tpControlDialogDef
{
	private:
        double  m_dBoundaryPointRangeRingSteps;
        bool    m_bCreateBoundaryPointHasFocus;
        bool    m_bCreateBoundaryHasFocus;
        wxFontDialog  *m_pfdDialog;
        wxFont  m_DisplayTextFont;

	protected:
        // Handlers for ODNewODPointDialogDef events.
        void tpControlOnClickImportJSON( wxCommandEvent& event );
        void tpControlOKClick( wxCommandEvent& event );
        void tpControlCloseClick( wxCommandEvent& event );
        void OnButtonClickFonts( wxCommandEvent& event );
        void OnButtonClickCreateBoundaryODAPI( wxCommandEvent& event );
        void OnButtonClickDeleteBoundaryODAPI( wxCommandEvent& event );
        void OnButtonClickCreateBoundaryPointODAPI( wxCommandEvent& event );
        void OnButtonClickDeleteBoundaryPointODAPI( wxCommandEvent& event );
        void OnButtonClickCreateTextPointODAPI( wxCommandEvent& event );
        void OnButtonClickDeleteTextPointODAPI( wxCommandEvent& event );
        void OnButtonClickPointIconODAPI( wxCommandEvent& event );
        void OnButtonClickCreateBoundaryJSON( wxCommandEvent& event );
        void OnButtonClickCreateBoundaryPointJSON( wxCommandEvent& event );
        void OnButtonClickCreateTextPointJSON( wxCommandEvent& event );
        void OnButtonClickDeleteBoundaryJSON( wxCommandEvent& event );
        void OnButtonClickDeleteBoundaryPointJSON( wxCommandEvent& event );
        void OnButtonClickDeleteTextPointJSON( wxCommandEvent& event );
        void OnButtonClickPointIconJSON( wxCommandEvent& event );
        void OnCheckBoxSaveJSONOnStartup( wxCommandEvent& event );
        void OnFileChangeInputJSON( wxFileDirPickerEvent& event );
        void OnFileChangeOutputJSON( wxFileDirPickerEvent& event );
        void OnCheckBoxSaveJSON( wxCommandEvent& event );
        void OnCheckBoxCloseSaveFileAfterEachWrite( wxCommandEvent& event );
        void OnCheckBoxAppendToFile( wxCommandEvent& event );
        void OnCheckBoxDeleteFromConfig( wxCommandEvent& event );


	public:
        tpControlDialogImpl( wxWindow *parent );
        void        SetDialogSize( void );
        void        SetLatLon( double lat, double lon );
        void        SetPanels(void);
        wxString    GetJSONSaveFile( void );
        void        SetJSONSaveFile( wxString SaveFile );
        wxString    GetJSONInputFile( void );
        void        SetJSONInputFile( wxString InputFile );
        void        SetSaveJSONOnStartup(bool SaveJSONOnStartup);
        void        SetIncommingJSONMessages(bool IncommingJSONMessages);
        void        SetAppendToSaveFile(bool AppendToSaveFile);
        void        SetCloseFileAfterEachWrite(bool CloseFileAfterEachWrite);

        bool m_bOK;
        int  m_iSelection;


};

#endif //__ODNEWODPOINTDIALOGIMPL_H__
