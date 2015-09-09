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


#include "repo_worker_collection.h"

#include "../logger/repo_logger.h"

#include <repo/core/model/bson/repo_bson_element.h>
#include <repo/core/model/bson/repo_node_mesh.h>
#include <repo/core/model/repo_node_utils.h>

using namespace repo::worker;
namespace repoModel = repo::core::model;

CollectionWorker::CollectionWorker(
	repo::RepoController *controller,
	repo::RepoToken      *token,
	const std::string    &database,
	const std::string    &collection)
	: controller(controller)
	, token(token)
	, database(database)
	, collection(collection) {}

CollectionWorker::~CollectionWorker() {}

void CollectionWorker::run()
{
	int jobsCount = 0;
	// undetermined (moving) progress bar
	emit progressRangeChanged(0, 0);
	emit progressValueChanged(0);


	jobsCount = controller->countItemsInCollection(token, database, collection);
	emit progressRangeChanged(0, jobsCount);

	//----------------------------------------------------------------------
	// Retrieves all BSON objects until finished or cancelled.
	uint64_t retrieved = 0;

	while (!cancelled && jobsCount > retrieved)
	{

		std::vector<repoModel::RepoBSON> bsons = 
			controller->getAllFromCollectionContinuous(token, database, collection, retrieved);

		for (auto bson : bsons)
		{
			if (cancelled) break;
			if (bson.isValid() && !bson.isEmpty())
			{
				const char *bsonType = bson.getStringField(REPO_NODE_LABEL_TYPE);
				QString type = QString(bsonType);
				emit keyValuePairAdded(++retrieved, (uint64_t)bson.objsize(), type.isEmpty() ? "BSONObj" : type, 0);
				decodeRecords(bson, 1);
				emit progressValueChanged(retrieved);
			}
		}
	}

	//--------------------------------------------------------------------------
	emit progressValueChanged(jobsCount);
	emit RepoAbstractWorker::finished();
}

void CollectionWorker::decodeRecords(const repoModel::RepoBSON& bson, uint32_t depth)
{
	std::set<std::string> fields;
	bson.getFieldNames(fields);
	for (std::set<std::string>::iterator field = fields.begin();
		!cancelled && field != fields.end();
		++field)
	{
		std::string fieldName = *field;
		repoModel::RepoBSONElement element = bson.getField(fieldName);
		QVariant key;
		QVariant value;
		QVariant type;

		bool ok;
		QString keyString = QString::fromStdString(fieldName);
		quint64 keyULongLong = keyString.toULongLong(&ok);
		if (ok)
			key = keyULongLong;
		else
			key = keyString;

		QDateTime datetime; // temp variable only!

		bool recursion = false;
		switch (element.type())
		{
			case repoModel::ElementType::ARRAY: // array
				value = (uint64_t) element.embeddedObject().objsize(); //nFields();
				type = QString("BSONArray");
				// TODO: colors and textures
				// else recursion
				recursion = true;
				break;
			case repoModel::ElementType::UUID: // binary data 
				value = QUuid(QString::fromStdString(
					UUIDtoString(bson.getUUIDField(fieldName))));
				type = QString("BSONUuid");
				break;
			case repoModel::ElementType::BINARY:
				//check if the bson is a mesh node

				if (bson.hasField(REPO_NODE_LABEL_TYPE) && REPO_NODE_TYPE_MESH ==
					bson.getField(REPO_NODE_LABEL_TYPE).str())
				{

					//FIXME: considering we're only dumping out the first and last value,
					// there is no reason to retrieve the whole thing via memcpy, which may be 
					// massive!
					repoModel::MeshNode mesh = (repoModel::MeshNode) bson;
					if (REPO_NODE_MESH_LABEL_VERTICES == key)
					{
						std::vector<repo_vector_t> * vec = mesh.getVertices();
						value = QString(vectorToString(*vec).c_str());
						type = QString("BSONBinDataGeneral");
						delete vec;
					}
					else if (REPO_NODE_MESH_LABEL_NORMALS == key)
					{
						std::vector<repo_vector_t> * vec = mesh.getNormals();
						value = QString(vectorToString(*vec).c_str());
						type = QString("BSONBinDataGeneral");
						delete vec;
					}
					else if (REPO_NODE_MESH_LABEL_UV_CHANNELS == key)
					{
						std::vector<repo_vector2d_t> * vec = mesh.getUVChannels();
						value = QString(vectorToString(*vec).c_str());
						type = QString("BSONBinDataGeneral");
						delete vec;
					}
					else if (REPO_NODE_MESH_LABEL_FACES == key)
					{
						std::vector<repo_face_t> * vec = mesh.getFaces();
						value = QString(vectorToString(*vec).c_str());
						type = QString("BSONBinDataGeneral");
						delete vec;
					}
					else
					{
						//Not recognised binary
						value = QString::fromStdString(element.toString());
						type = QString("BSONBinData");
					}
				}
				else
				{
					//Not mesh node - generic binary
					value = QString::fromStdString(element.toString());
					type = QString("BSONBinData");
				}
				break;
			case repoModel::ElementType::BOOL:
				value = element.booleanSafe();
				type = QString("BSONBool");
				break;
			case repoModel::ElementType::DATE:
				datetime.setMSecsSinceEpoch(element.Date());
				value = datetime;
				type = QString("BSONDate");
				break;
			case repoModel::ElementType::OBJECTID: // ObjectId
				value = QString::fromStdString(element.OID().toString());
				type = QString("BSONObjID");
				break;
			case repoModel::ElementType::DOUBLE: // double precision floating point value
				value = element.numberDouble();
				type = QString("BSONDouble");
				break;
			case repoModel::ElementType::INT: // 32 bit signed integer 
				value = element.numberInt();
				type = QString("BSONInt32");
				break;
			case repoModel::ElementType::LONG: // 64 bit signed integer
				value = element.numberLong();
				type = QString("BSONInt64");
				break;
			case repoModel::ElementType::OBJECT: // embedded object
				value = (unsigned long long) element.embeddedObject().objsize(); //nFields();
				type = QString("BSONObj");
				recursion = true;
				break;
			case repoModel::ElementType::STRING: // character string, stored in utf8 
				value = QString::fromStdString(element.String());
				type = QString("BSONString");
				break;
			default:
				value = QString::fromStdString(element.toString());
				type = QString("Undefined");
				break;
		}

		emit keyValuePairAdded(key, value, type, depth);
		if (!cancelled && recursion)
			decodeRecords(element.embeddedObject(), depth + 1);
	}
}
