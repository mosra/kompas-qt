#ifndef Kompas_Plugins_UIComponents_AreaPage_h
#define Kompas_Plugins_UIComponents_AreaPage_h
/*
    Copyright © 2007, 2008, 2009, 2010, 2011 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Kompas.

    Kompas is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Kompas is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Class Kompas::Plugins::UIComponents::AreaPage
 */

#include <QtGui/QWizardPage>

class QRadioButton;

namespace Kompas { namespace Plugins { namespace UIComponents {

class SaveRasterWizard;

/**
 * @brief Area selection wizard page
 *
 * Provides selection of whole map, visible area or custom specified area.
 * @see SaveRasterWizard::AreaType
 */
class AreaPage: public QWizardPage {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param _wizard       Wizard instance
         */
        AreaPage(SaveRasterWizard* _wizard);

        /**
         * @brief Page validator
         *
         * Saves selected area type into SaveRasterWizard::areaType.
         */
        bool validatePage();

    private:
        SaveRasterWizard* wizard;

        QRadioButton *wholeMap,
            *visibleArea,
            *customArea;

        QPushButton* customAreaSelect;
};

}}}

#endif
