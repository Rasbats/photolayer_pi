/***************************************************************************
 *
 * Project:  OpenCPN PhotoLayer plugin
 * Author:   Mike Rossiter, based on WeatherFax by Sean D'Epagnier
 *
 ***************************************************************************
 *   Copyright (C) 2017 by Mike Rossiter                                   *
 *                                                                         *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.             *
 ***************************************************************************
 */

#ifndef _PHOTOLAYER_ABOUT_H_
#define _PHOTOLAYER_ABOUT_H_

#include "PhotoLayerUI.h"

class AboutDialog : public AboutDialogBase
{
public:
    AboutDialog( wxWindow *parent );
    void OnAboutAuthor( wxCommandEvent& event );
    void OnClose( wxCommandEvent& event ) { EndModal(wxID_OK); }   
};

#endif
