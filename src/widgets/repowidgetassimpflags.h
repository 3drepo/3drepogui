/**
 *  Copyright (C) 2015 3D Repo Ltd
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef REPO_WIDGET_ASSIMP_FLAGS_H
#define REPO_WIDGET_ASSIMP_FLAGS_H

//------------------------------------------------------------------------------
// Qt
#include <QWidget>
#include <QSettings>

//------------------------------------------------------------------------------
// Core
#include <RepoWrapperAssimp>
#include <assimp\postprocess.h>

namespace Ui {
class RepoWidgetAssimpFlags;
}

namespace repo {
namespace gui {

//! See http://assimp.sourceforge.net/lib_html/postprocess_8h.html
class RepoWidgetAssimpFlags : public QWidget
{
    Q_OBJECT

public:

    explicit RepoWidgetAssimpFlags(QWidget *parent = 0);

    ~RepoWidgetAssimpFlags();

public :

    //! Saves the current state of the flags into settings.
    void apply();

    /*!
     * Returns the debone threshold set in settings. Defaults to Assimp's
     * AI_DEBONE_THRESHOLD = 1.0f
     */
    float getDeboneThreshold() const
    { return settings.value(REPO_SETTINGS_DEBONE_THRESHOLD,
                            AI_DEBONE_THRESHOLD).toFloat(); }

    //! Returns a bit-wise or of currently set Assimp post-processing flags.
    unsigned int getPostProcessingFlags(unsigned int flag = 0) const;

    /*!
     * Returns the animation accuracy for find invalid data set in settings.
     * Defaults to 0 == all comparisons are exact.
     * See http://assimp.sourceforge.net/lib_html/config_8h.html#ad223c5e7e63d2937685cc704a181b950
     */
    float getFindInvalidDataAnimationAccuracy() const
    { return settings.value(REPO_SETTINGS_FIND_INAVLID_DATA_ANIMATION_ACCURACY,
                            0).toFloat(); }

    /*!
     * Returns true if calculate tangent space is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isCalculateTangentSpaceChecked() const
    { return settings.value(REPO_SETTINGS_CALCULATE_TANGENT_SPACE,
                            false).toBool();}

    /*!
     * Returns true if convert to UV coords is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isConvertToUVCoordinatesChecked() const
    { return settings.value(REPO_SETTINGS_CONVERT_TO_UV_COORDINATES,
                            false).toBool(); }

    /*!
     * Returns true if degenerates to points/lines is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isDegeneratesToPointsLinesChecked() const
    { return  settings.value(REPO_SETTINGS_DEGENERATES_TO_POINTS_LINES,
                             false).toBool(); }

    /*!
     * Returns true if debone is checked in settings, false otherwise. Defaults
     * to false.
     */
    bool isDeboneChecked() const
    { return settings.value(REPO_SETTINGS_DEBONE, false).toBool(); }

    /*!
     * Returns true if debone only if all is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isDeboneOnlyIfAllChecked() const
    { return settings.value(REPO_SETTINGS_DEBONE_ONLY_IF_ALL, false).toBool(); }

    /*!
     * Returns true if find instances is checked in settings, false otherwise.
     * Defaults to false.
     */
    bool isFindInstancesChecked() const
    { return settings.value(REPO_SETTINGS_FIND_INSTANCES, false).toBool(); }

    /*!
     * Returns true if find invalid data is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isFindInvalidDataChecked() const
    { return settings.value(REPO_SETTINGS_FIND_INAVLID_DATA, false).toBool(); }

    /*!
     * Returns true if fix infacing normals is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isFixInfacingNormalsChecked() const
    { return settings.value(REPO_SETTINGS_FIX_INFACING_NORMALS, false).toBool(); }

    /*!
     * Returns true if flip UV coordinates is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isFlipUVCoordinatesChecked() const
    { return settings.value(REPO_SETTINGS_FLIP_UV_COORDINATES, false).toBool(); }

    /*!
     * Returns true if flip winding order is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isFlipWindingOrderChecked() const
    { return settings.value(REPO_SETTINGS_FLIP_WINDING_ORDER, false).toBool(); }

    /*!
     * Returns true if generate normals is checked in settings, false otherwise.
     * Defaults to false.
     */
    bool isGenerateNormalsChecked() const
    { return settings.value(REPO_SETTINGS_GENERATE_NORMALS, false).toBool(); }

    /*!
     * Returns true if generate flat normals is checked in settings, false
     * oterwise. Defaults to true.
     */
    bool isGenerateNormalsFlatChecked() const
    { return settings.value(REPO_SETTINGS_GENERATE_NORMALS_FLAT, true).toBool(); }

    /*!
     * Returns true if generate smooth normals is checked in settings, false
     * otherwise. Defaults to false.
     */
    bool isGenerateNormalsSmoothChecked() const
    { return settings.value(REPO_SETTINGS_GENERATE_NORMALS_SMOOTH, false).toBool(); }

public :

    static const QString REPO_SETTINGS_CALCULATE_TANGENT_SPACE;

    static const QString REPO_SETTINGS_CONVERT_TO_UV_COORDINATES;

    static const QString REPO_SETTINGS_DEGENERATES_TO_POINTS_LINES;

    static const QString REPO_SETTINGS_DEBONE;

    static const QString REPO_SETTINGS_DEBONE_THRESHOLD;

    static const QString REPO_SETTINGS_DEBONE_ONLY_IF_ALL;

    static const QString REPO_SETTINGS_FIND_INSTANCES;

    static const QString REPO_SETTINGS_FIND_INAVLID_DATA;

    static const QString REPO_SETTINGS_FIND_INAVLID_DATA_ANIMATION_ACCURACY;

    static const QString REPO_SETTINGS_FIX_INFACING_NORMALS;

    static const QString REPO_SETTINGS_FLIP_UV_COORDINATES;

    static const QString REPO_SETTINGS_FLIP_WINDING_ORDER;

    static const QString REPO_SETTINGS_GENERATE_NORMALS;

    static const QString REPO_SETTINGS_GENERATE_NORMALS_FLAT;

    static const QString REPO_SETTINGS_GENERATE_NORMALS_SMOOTH;


private:

    //! UI var.
    Ui::RepoWidgetAssimpFlags *ui;

    //! Application settings.
    QSettings settings;

};

} // end namespace gui
} // end namespace repo

#endif // REPO_WIDGET_ASSIMP_FLAGS_H
