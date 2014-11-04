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


#ifndef REPO_WORKER_ASSIMP_H
#define REPO_WORKER_ASSIMP_H

//-----------------------------------------------------------------------------
#include "repo_worker_abstract.h"
#include "../conversion/repo_transcoder_assimp.h"
//-----------------------------------------------------------------------------
#include "assimpwrapper.h"
#include "graph/repo_graph_scene.h"
//-----------------------------------------------------------------------------
#include <QImage>

namespace repo {
namespace gui {

/*!
 * Worker class to load a 3D file using the Assimp loading library. Use with 
 * QThread by connecting signals and slots. The worker needs to be moved to a
 * new thread and the thread has to be started.
 */
class RepoWorkerAssimp : public RepoWorkerAbstract {

	Q_OBJECT

public :

	/*!
	 * Constructor. Takes a full file path to a 3D file that is to be loaded
	 * using the process() function and Assimp loading flags.
	 *
	 * \sa ~RepoWorkerAssimp(), process()
	 */
	RepoWorkerAssimp(const QString& fullPath, const unsigned int& pFlags = 0);

	//! Default empty destructor.
	~RepoWorkerAssimp();

	//! Returns a file name from a full file path.
	static QString getFileName(const QString& fullPath);

	//! Load textures stored locally at the base path.
	static std::map<std::string, QImage> loadTextures(
		const aiScene * scene, 
		const std::string & basePath);

	static std::map<std::string, repo::core::RepoNodeAbstract *> loadTextures(
		const std::map<string, QImage> &qtextures,
		const std::string &texturesFolderPath);

public slots :

	/*! 
	 * Loads scene and emits signals as the loading progresses and once
	 * the loading is finished.
	 */
	void run();

signals :

	//! Emitted when loading is finished. Passes Repo scene and GLC world.
	void finished(repo::core::RepoGraphScene *, GLC_World &);

private :

	//! Full canonical path of the 3D file to be loaded.
	const QString fullPath;

	//! Assimp loading flags.
	const unsigned int pFlags;
 
}; // end class

} // end namespace core
} // end namespace repo

#endif // end REPO_WORKER_ASSIMP_H
