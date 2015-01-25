/**
 *  Copyright (C) 2014 3D Repo Ltd
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

#ifndef REPO_TRANSCODER_GRAPH_SCENE_H
#define REPO_TRANSCODER_GRAPH_SCENE_H

#include <string>
//------------------------------------------------------------------------------
#include <assimp/scene.h> 
//------------------------------------------------------------------------------
#include <QObject>
#include <QString>
#include <QFile>
#include <QXmlStreamReader>
#include <QHash>
#include <QColor>
//------------------------------------------------------------------------------
#include <GLC_Material>
#include <GLC_World>
#include "glc_config.h"
#include "geometry/glc_mesh.h"
#include "sceneGraph/glc_structoccurence.h"
#include "maths/glc_geomtools.h"
#include <RepoGraphScene>
//------------------------------------------------------------------------------
namespace repo {
namespace gui {

/*!
 * Creates GLC_World instance out of Repo Graph scene
 */
class RepoTranscoderGraphScene
{
public:

	//! Creates a world instance of a given scene.
	static GLC_World toGLCWorld(
        const core::RepoGraphScene *,
		const std::map<std::string, QImage> &,
		const std::string & namePrefix = "");

	//! Creates a struct occurrence instance out of a given node.
	/*!
	 * Recursive function to create a hierarchy of occurrences of a given 
	 * Assimp node and all of its children. Call with root node to process
	 * the entire scene graph.
	 */
	static GLC_StructOccurence* createOccurrenceFromNode(
        const core::RepoGraphScene* scene,
        const core::RepoNodeAbstract* node,
        const std::map<boost::uuids::uuid, GLC_3DRep*>& glcMeshes,
        const QHash<const QString, GLC_3DRep*> &glcCameras);

    /*!
     * Returns a GLC Material given a RepoNodeAbstract node and a fileName map of
     * textures.
     */
    static GLC_Material * toGLCMaterial(core::RepoNodeAbstract*, const QHash<QString, GLC_Texture>&);

    //! Returns a GLC 3DRep given a RepoNodeAbstract and a vector of GLC Materials.
    static GLC_3DRep* toGLCMesh(core::RepoNodeAbstract*, const QVector<GLC_Material*> &,
        const std::string &namePrefix);

    //! Returns a GLC Mesh given a RepoNodeAbstract
    static GLC_3DRep* toGLCCamera(core::RepoNodeAbstract*);

}; // end class

} // end namespace gui
} // end namespace repo

#endif // end REPO_TRANSCODER_GRAPH_SCENE_H
