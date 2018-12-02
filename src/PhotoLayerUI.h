///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/slider.h>
#include <wx/checkbox.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/stattext.h>
#include <wx/spinctrl.h>
#include <wx/radiobut.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class PhotoLayerBase
///////////////////////////////////////////////////////////////////////////////
class PhotoLayerBase : public wxFrame
{
	private:

	protected:
		wxCheckBox* m_cInvert;
		wxMenuBar* m_menubar1;
		wxMenu* m_menu1;
		wxMenuItem* m_mSaveAs;
		wxMenuItem* m_mEdit;
		wxMenuItem* m_mGoto;
		wxMenuItem* m_mExport;
		wxMenuItem* m_mDelete;
		wxMenu* m_menu3;

		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnFaxes( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEdit( wxCommandEvent& event ) { event.Skip(); }
		virtual void TransparencyChanged( wxScrollEvent& event ) { event.Skip(); }
		virtual void WhiteTransparencyChanged( wxScrollEvent& event ) { event.Skip(); }
		virtual void OnInvert( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOpen( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSaveAs( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnGoto( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnExport( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDelete( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPreferences( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClose( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAbout( wxCommandEvent& event ) { event.Skip(); }


	public:
		wxStaticBoxSizer* sbFax;
		wxListBox* m_lFaxes;
		wxSlider* m_sTransparency;
		wxSlider* m_sWhiteTransparency;

		PhotoLayerBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("PhotoLayer"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxCAPTION|wxCLOSE_BOX|wxFRAME_FLOAT_ON_PARENT|wxFRAME_NO_TASKBAR|wxRESIZE_BORDER|wxSYSTEM_MENU|wxTAB_TRAVERSAL );

		~PhotoLayerBase();

};

///////////////////////////////////////////////////////////////////////////////
/// Class PhotoLayerPrefsDialog
///////////////////////////////////////////////////////////////////////////////
class PhotoLayerPrefsDialog : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText36;
		wxStaticText* m_staticText37;
		wxStaticText* m_staticText45;
		wxStaticText* m_staticText38;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Cancel;

	public:
		wxSpinCtrl* m_sExportColors;
		wxRadioButton* m_rbExportDepthMeters;
		wxRadioButton* m_rbExportDepthFathoms;
		wxTextCtrl* m_tExportSoundingDatum;

		PhotoLayerPrefsDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("PhotoLayer Preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION|wxDEFAULT_DIALOG_STYLE );
		~PhotoLayerPrefsDialog();

};

///////////////////////////////////////////////////////////////////////////////
/// Class AboutDialogBase
///////////////////////////////////////////////////////////////////////////////
class AboutDialogBase : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText34;
		wxStaticText* m_stVersion;
		wxStaticText* m_staticText110;
		wxButton* m_bAboutAuthor;
		wxButton* m_bClose;

		// Virtual event handlers, overide them in your derived class
		virtual void OnAboutAuthor( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClose( wxCommandEvent& event ) { event.Skip(); }


	public:

		AboutDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("About PhotoLayer"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~AboutDialogBase();

};

