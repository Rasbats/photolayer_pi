///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 21 2016)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "PhotoLayerUI.h"

///////////////////////////////////////////////////////////////////////////

PhotoLayerBase::PhotoLayerBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* fgSizer8;
	fgSizer8 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer8->AddGrowableCol( 0 );
	fgSizer8->AddGrowableRow( 0 );
	fgSizer8->SetFlexibleDirection( wxBOTH );
	fgSizer8->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	sbFax = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Layer") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer7;
	fgSizer7 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer7->AddGrowableCol( 0 );
	fgSizer7->AddGrowableRow( 0 );
	fgSizer7->SetFlexibleDirection( wxBOTH );
	fgSizer7->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_lFaxes = new wxListBox( sbFax->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxSize( -1,100 ), 0, NULL, wxLB_EXTENDED ); 
	m_lFaxes->SetBackgroundColour( wxColour( 255, 198, 255 ) );
	
	fgSizer7->Add( m_lFaxes, 0, wxALL|wxEXPAND, 5 );
	
	
	sbFax->Add( fgSizer7, 1, wxEXPAND, 5 );
	
	
	fgSizer8->Add( sbFax, 1, wxEXPAND, 0 );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Transparency") ), wxVERTICAL );
	
	m_sTransparency = new wxSlider( sbSizer4->GetStaticBox(), wxID_ANY, 50, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	m_sTransparency->SetBackgroundColour( wxColour( 250, 203, 107 ) );
	
	sbSizer4->Add( m_sTransparency, 0, wxEXPAND, 5 );
	
	m_sWhiteTransparency = new wxSlider( sbSizer4->GetStaticBox(), wxID_ANY, 150, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	m_sWhiteTransparency->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
	m_sWhiteTransparency->SetBackgroundColour(wxColour( 250, 203, 107 ));
	
	sbSizer4->Add( m_sWhiteTransparency, 0, wxEXPAND, 5 );
	
	
	fgSizer8->Add( sbSizer4, 1, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer24;
	fgSizer24 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer24->SetFlexibleDirection( wxBOTH );
	fgSizer24->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_cInvert = new wxCheckBox( this, wxID_ANY, _("Invert"), wxDefaultPosition, wxSize( 300,-1 ), 0 );
	fgSizer24->Add( m_cInvert, 0, wxALL, 5 );
	
	
	fgSizer8->Add( fgSizer24, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( fgSizer8 );
	this->Layout();
	fgSizer8->Fit( this );
	m_menubar1 = new wxMenuBar( 0 );
	m_menu1 = new wxMenu();
	wxMenuItem* m_mOpen;
	m_mOpen = new wxMenuItem( m_menu1, wxID_ANY, wxString( _("&Open") ) + wxT('\t') + wxT("Ctrl+o"), wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_mOpen );
	
	m_mSaveAs = new wxMenuItem( m_menu1, wxID_ANY, wxString( _("Save &As") ) + wxT('\t') + wxT("Ctrl+S"), wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_mSaveAs );
	
	m_mGoto = new wxMenuItem( m_menu1, wxID_ANY, wxString( _("&Goto") ) + wxT('\t') + wxT("Ctrl+g"), wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_mGoto );

	m_mExport = new wxMenuItem(m_menu1, wxID_ANY, wxString(_("&Export")) + wxT('\t') + wxT("Ctrl+e"), wxEmptyString, wxITEM_NORMAL);
	m_menu1->Append(m_mExport);
	
	m_mDelete = new wxMenuItem( m_menu1, wxID_ANY, wxString( _("&Delete") ) + wxT('\t') + wxT("Ctrl+d"), wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_mDelete );
	
	m_menu1->AppendSeparator();
	
	wxMenuItem* m_menuItem9;
	m_menuItem9 = new wxMenuItem( m_menu1, wxID_ANY, wxString( _("&Preferences") ) + wxT('\t') + wxT("Ctrl+p"), wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_menuItem9 );
	
	m_menu1->AppendSeparator();
	
	wxMenuItem* m_menuItem4;
	m_menuItem4 = new wxMenuItem( m_menu1, wxID_ANY, wxString( _("&Close") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_menuItem4 );
	
	m_menubar1->Append( m_menu1, _("&File") ); 
	
	m_menu3 = new wxMenu();
	wxMenuItem* m_menuItem7;
	m_menuItem7 = new wxMenuItem( m_menu3, wxID_ANY, wxString( _("&About") ) + wxT('\t') + wxT("F1"), wxEmptyString, wxITEM_NORMAL );
	m_menu3->Append( m_menuItem7 );
	
	m_menubar1->Append( m_menu3, _("&Help") ); 
	
	this->SetMenuBar( m_menubar1 );
	
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( PhotoLayerBase::OnClose ) );
	m_lFaxes->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( PhotoLayerBase::OnFaxes ), NULL, this );
	m_lFaxes->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( PhotoLayerBase::OnEdit ), NULL, this );
	m_sTransparency->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( PhotoLayerBase::TransparencyChanged ), NULL, this );
	m_sTransparency->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( PhotoLayerBase::TransparencyChanged ), NULL, this );
	m_sTransparency->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( PhotoLayerBase::TransparencyChanged ), NULL, this );
	m_sTransparency->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( PhotoLayerBase::TransparencyChanged ), NULL, this );
	m_sTransparency->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( PhotoLayerBase::TransparencyChanged ), NULL, this );
	m_sTransparency->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( PhotoLayerBase::TransparencyChanged ), NULL, this );
	m_sTransparency->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( PhotoLayerBase::TransparencyChanged ), NULL, this );
	m_sTransparency->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( PhotoLayerBase::TransparencyChanged ), NULL, this );
	m_sTransparency->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( PhotoLayerBase::TransparencyChanged ), NULL, this );
	m_sWhiteTransparency->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( PhotoLayerBase::WhiteTransparencyChanged ), NULL, this );
	m_sWhiteTransparency->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( PhotoLayerBase::WhiteTransparencyChanged ), NULL, this );
	m_sWhiteTransparency->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( PhotoLayerBase::WhiteTransparencyChanged ), NULL, this );
	m_sWhiteTransparency->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( PhotoLayerBase::WhiteTransparencyChanged ), NULL, this );
	m_sWhiteTransparency->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( PhotoLayerBase::WhiteTransparencyChanged ), NULL, this );
	m_sWhiteTransparency->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( PhotoLayerBase::WhiteTransparencyChanged ), NULL, this );
	m_sWhiteTransparency->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( PhotoLayerBase::WhiteTransparencyChanged ), NULL, this );
	m_sWhiteTransparency->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( PhotoLayerBase::WhiteTransparencyChanged ), NULL, this );
	m_sWhiteTransparency->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( PhotoLayerBase::WhiteTransparencyChanged ), NULL, this );
	m_cInvert->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PhotoLayerBase::OnInvert ), NULL, this );
	this->Connect( m_mOpen->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PhotoLayerBase::OnOpen ) );
	this->Connect( m_mSaveAs->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PhotoLayerBase::OnSaveAs ) );
	
	this->Connect( m_mGoto->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PhotoLayerBase::OnGoto ) );
	this->Connect(m_mExport->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(PhotoLayerBase::OnExport));
	this->Connect( m_mDelete->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PhotoLayerBase::OnDelete ) );
	this->Connect( m_menuItem9->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PhotoLayerBase::OnPreferences ) );
	this->Connect( m_menuItem4->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PhotoLayerBase::OnClose ) );
	this->Connect( m_menuItem7->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PhotoLayerBase::OnAbout ) );
}

PhotoLayerBase::~PhotoLayerBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( PhotoLayerBase::OnClose ) );
	m_lFaxes->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( PhotoLayerBase::OnFaxes ), NULL, this );
	m_lFaxes->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( PhotoLayerBase::OnEdit ), NULL, this );
	m_sTransparency->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( PhotoLayerBase::TransparencyChanged ), NULL, this );
	m_sTransparency->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( PhotoLayerBase::TransparencyChanged ), NULL, this );
	m_sTransparency->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( PhotoLayerBase::TransparencyChanged ), NULL, this );
	m_sTransparency->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( PhotoLayerBase::TransparencyChanged ), NULL, this );
	m_sTransparency->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( PhotoLayerBase::TransparencyChanged ), NULL, this );
	m_sTransparency->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( PhotoLayerBase::TransparencyChanged ), NULL, this );
	m_sTransparency->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( PhotoLayerBase::TransparencyChanged ), NULL, this );
	m_sTransparency->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( PhotoLayerBase::TransparencyChanged ), NULL, this );
	m_sTransparency->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( PhotoLayerBase::TransparencyChanged ), NULL, this );
	m_sWhiteTransparency->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( PhotoLayerBase::WhiteTransparencyChanged ), NULL, this );
	m_sWhiteTransparency->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( PhotoLayerBase::WhiteTransparencyChanged ), NULL, this );
	m_sWhiteTransparency->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( PhotoLayerBase::WhiteTransparencyChanged ), NULL, this );
	m_sWhiteTransparency->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( PhotoLayerBase::WhiteTransparencyChanged ), NULL, this );
	m_sWhiteTransparency->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( PhotoLayerBase::WhiteTransparencyChanged ), NULL, this );
	m_sWhiteTransparency->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( PhotoLayerBase::WhiteTransparencyChanged ), NULL, this );
	m_sWhiteTransparency->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( PhotoLayerBase::WhiteTransparencyChanged ), NULL, this );
	m_sWhiteTransparency->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( PhotoLayerBase::WhiteTransparencyChanged ), NULL, this );
	m_sWhiteTransparency->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( PhotoLayerBase::WhiteTransparencyChanged ), NULL, this );
	m_cInvert->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PhotoLayerBase::OnInvert ), NULL, this );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PhotoLayerBase::OnOpen ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PhotoLayerBase::OnSaveAs ) );
	
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PhotoLayerBase::OnGoto ) );
	this->Disconnect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(PhotoLayerBase::OnExport));
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PhotoLayerBase::OnDelete ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PhotoLayerBase::OnPreferences ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PhotoLayerBase::OnClose ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PhotoLayerBase::OnAbout ) );
	
}

PhotoLayerPrefsDialog::PhotoLayerPrefsDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* fgSizer51;
	fgSizer51 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer51->SetFlexibleDirection( wxBOTH );
	fgSizer51->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* fgSizer55;
	fgSizer55 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer55->SetFlexibleDirection( wxBOTH );
	fgSizer55->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* fgSizer56;
	fgSizer56 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer56->SetFlexibleDirection( wxBOTH );
	fgSizer56->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* sbSizer17;
	sbSizer17 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Export") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer53;
	fgSizer53 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer53->SetFlexibleDirection( wxBOTH );
	fgSizer53->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* fgSizer511;
	fgSizer511 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer511->SetFlexibleDirection( wxBOTH );
	fgSizer511->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText36 = new wxStaticText( sbSizer17->GetStaticBox(), wxID_ANY, _("Reduce to"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText36->Wrap( -1 );
	fgSizer511->Add( m_staticText36, 0, wxALL, 5 );
	
	m_sExportColors = new wxSpinCtrl( sbSizer17->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 2, 32768, 64 );
	fgSizer511->Add( m_sExportColors, 0, wxALL, 5 );
	
	m_staticText37 = new wxStaticText( sbSizer17->GetStaticBox(), wxID_ANY, _("Colors"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText37->Wrap( -1 );
	fgSizer511->Add( m_staticText37, 0, wxALL, 5 );
	
	
	fgSizer53->Add( fgSizer511, 1, wxEXPAND, 5 );
	
	m_staticText45 = new wxStaticText( sbSizer17->GetStaticBox(), wxID_ANY, _("Recommended 4, 16, 64, or 256"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText45->Wrap( -1 );
	fgSizer53->Add( m_staticText45, 0, wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer181;
	sbSizer181 = new wxStaticBoxSizer( new wxStaticBox( sbSizer17->GetStaticBox(), wxID_ANY, _("Depth") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer52;
	fgSizer52 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer52->SetFlexibleDirection( wxBOTH );
	fgSizer52->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_rbExportDepthMeters = new wxRadioButton( sbSizer181->GetStaticBox(), wxID_ANY, _("Meters"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer52->Add( m_rbExportDepthMeters, 0, wxALL, 5 );
	
	m_rbExportDepthFathoms = new wxRadioButton( sbSizer181->GetStaticBox(), wxID_ANY, _("Fathoms"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer52->Add( m_rbExportDepthFathoms, 0, wxALL, 5 );
	
	
	sbSizer181->Add( fgSizer52, 1, wxEXPAND, 5 );
	
	
	fgSizer53->Add( sbSizer181, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer54;
	fgSizer54 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer54->SetFlexibleDirection( wxBOTH );
	fgSizer54->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText38 = new wxStaticText( sbSizer17->GetStaticBox(), wxID_ANY, _("fix sounding datum"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText38->Wrap( -1 );
	fgSizer54->Add( m_staticText38, 0, wxALL, 5 );
	
	m_tExportSoundingDatum = new wxTextCtrl( sbSizer17->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer54->Add( m_tExportSoundingDatum, 0, wxALL, 5 );
	
	
	fgSizer53->Add( fgSizer54, 1, wxEXPAND, 5 );
	
	
	sbSizer17->Add( fgSizer53, 1, wxEXPAND, 5 );
	
	
	fgSizer56->Add( sbSizer17, 1, wxEXPAND, 5 );
	
	
	fgSizer55->Add( fgSizer56, 1, wxEXPAND, 5 );
	
	
	fgSizer51->Add( fgSizer55, 1, wxEXPAND, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	
	fgSizer51->Add( m_sdbSizer1, 0, wxBOTTOM|wxEXPAND|wxTOP, 5 );
	
	
	this->SetSizer( fgSizer51 );
	this->Layout();
	fgSizer51->Fit( this );
	
	this->Centre( wxBOTH );
}

PhotoLayerPrefsDialog::~PhotoLayerPrefsDialog()
{
}

AboutDialogBase::AboutDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* fgSizer90;
	fgSizer90 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer90->SetFlexibleDirection( wxBOTH );
	fgSizer90->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* fgSizer50;
	fgSizer50 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer50->SetFlexibleDirection( wxBOTH );
	fgSizer50->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText34 = new wxStaticText( this, wxID_ANY, _("PhotoLayer Plugin Version"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText34->Wrap( -1 );
	fgSizer50->Add( m_staticText34, 0, wxALL, 5 );
	
	m_stVersion = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_stVersion->Wrap( -1 );
	fgSizer50->Add( m_stVersion, 0, wxALL, 5 );
	
	
	fgSizer90->Add( fgSizer50, 1, wxEXPAND, 5 );
	
	m_staticText110 = new wxStaticText( this, wxID_ANY, _("The PhotoLayer plugin for OpenCPN is intended for opening a GeoTiff photo overlay file and placing the embedded satellite photo directly onto a navigational chart.\n\nTo get started, open the GeoTiff you have available.\n\n Images can be downloaded from 'VentureFarther.com'\n\nSource Code:\nhttps://github.com/rasbats/photolayer_pi\n\nThe plugin code is largely based on the Weather Fax plugin from Sean D'Epagnier. The code for retrieving the geokeys from the GeoTiff file is due to massive help from Bill Stockton (Chartaid).\n\nMany thanks to 'VentureFarther', translators and testers."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText110->Wrap( 400 );
	fgSizer90->Add( m_staticText110, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer91;
	fgSizer91 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer91->SetFlexibleDirection( wxBOTH );
	fgSizer91->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_bAboutAuthor = new wxButton( this, wxID_ANY, _("About the Author"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer91->Add( m_bAboutAuthor, 0, wxALL, 5 );
	
	m_bClose = new wxButton( this, wxID_ANY, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer91->Add( m_bClose, 0, wxALL, 5 );
	
	
	fgSizer90->Add( fgSizer91, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( fgSizer90 );
	this->Layout();
	fgSizer90->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_bAboutAuthor->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AboutDialogBase::OnAboutAuthor ), NULL, this );
	m_bClose->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AboutDialogBase::OnClose ), NULL, this );
}

AboutDialogBase::~AboutDialogBase()
{
	// Disconnect Events
	m_bAboutAuthor->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AboutDialogBase::OnAboutAuthor ), NULL, this );
	m_bClose->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AboutDialogBase::OnClose ), NULL, this );
	
}
