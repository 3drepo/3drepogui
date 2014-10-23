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


#include "repo_workercollection.h"
#include <RepoNodeMesh>
#include <RepoTranscoderBSON>
#include <RepoTranscoderString>

repo::gui::RepoWorkerCollection::RepoWorkerCollection(
	const repo::core::MongoClientWrapper& mongo,
	const QString &database, 
	const QString &collection) 
	: mongo(mongo)
	, database(database.toStdString())
	, collection(collection.toStdString()) {}

repo::gui::RepoWorkerCollection::~RepoWorkerCollection() {}

void repo::gui::RepoWorkerCollection::run()
{	
	int jobsCount = 0;
	// undetermined (moving) progress bar
	emit progressRangeChanged(0, 0);
	emit progressValueChanged(0);

	if (!mongo.reconnect())
        std::cerr << "Connection failed" << std::endl;
	else
	{
		// Authentication can return false especially if it is not required by 
		// the remote DB.
		mongo.reauthenticate(database);	


		jobsCount = mongo.countItemsInCollection(database, collection);
		emit progressRangeChanged(0, jobsCount);

        //----------------------------------------------------------------------
		// Retrieves all BSON objects until finished or cancelled.
		unsigned long long retrieved = 0;
		std::auto_ptr<mongo::DBClientCursor> cursor;		
		do
		{
			for (; !cancelled && cursor.get() && cursor->more(); ++retrieved)
			{
				mongo::BSONObj bson = cursor->nextSafe();	
				QString type = QString(bson.getStringField(REPO_NODE_LABEL_TYPE));
				emit keyValuePairAdded(retrieved, (unsigned long long) bson.objsize(), type.isEmpty() ? "BSONObj" : type, 0);
				decodeRecords(bson, 1);
				emit progressValueChanged(retrieved);
			}
			if (!cancelled)
				cursor = mongo.listAllTailable(database, collection, retrieved);		
		}
		while (!cancelled && cursor.get() && cursor->more());
	}
    //--------------------------------------------------------------------------
	emit progressValueChanged(jobsCount);
	emit RepoWorkerAbstract::finished();
}

void repo::gui::RepoWorkerCollection::decodeRecords(const mongo::BSONObj& bson, unsigned int depth)
{
	std::set<std::string> fields;
	bson.getFieldNames(fields);	
	for(std::set<std::string>::iterator field = fields.begin(); 
		!cancelled && field != fields.end(); 
		++field) 
	{
		mongo::BSONElement element = bson.getField(*field);
		QVariant key;
		QVariant value;
		QVariant type;

		bool ok;
		QString keyString = QString::fromStdString(*field);
		quint64 keyULongLong = keyString.toULongLong(&ok);
		if (ok)
			key = keyULongLong;
		else
			key = keyString;

		QDateTime datetime; // temp variable only!

		bool recursion = false;
		switch(element.type())
		{
			case mongo::Array : // array
				value = (unsigned long long) element.embeddedObject().objsize(); //nFields();
				type = QString("BSONArray");
				// TODO: colors and textures
				// else recursion
				recursion = true;			
				break;
			case mongo::BinData : // binary data 					
				if (mongo::bdtUUID == element.binDataType()) 
				{
                    value = QUuid(QString::fromStdString(
                                core::MongoClientWrapper::uuidToString(
                                core::MongoClientWrapper::retrieveUUID(element))));
					type = QString("BSONUuid");
				}
				else if (REPO_NODE_LABEL_VERTICES == key)
				{
					const unsigned int verticesSize = bson.getField(REPO_NODE_LABEL_VERTICES_COUNT).numberInt();
					std::vector<aiVector3t<float>> * vec = new std::vector<aiVector3t<float>>();
					repo::core::RepoTranscoderBSON::retrieve(element, verticesSize, vec);
					value = QString(repo::core::RepoTranscoderString::toString(*vec).c_str());
					type = QString("BSONBinDataGeneral");
					delete vec;
				}
				else if (REPO_NODE_LABEL_NORMALS == key)
				{
					const unsigned int normalsSize = bson.getField(REPO_NODE_LABEL_VERTICES_COUNT).numberInt();
					std::vector<aiVector3t<float>> * vec = new std::vector<aiVector3t<float>>();
					repo::core::RepoTranscoderBSON::retrieve(element, normalsSize, vec);
					value = QString(repo::core::RepoTranscoderString::toString(*vec).c_str());
					type = QString("BSONBinDataGeneral");
					delete vec;
				}
				else if (REPO_NODE_LABEL_UV_CHANNELS == key)
				{
					const unsigned int uvChannelsSize = 
						bson.getField(REPO_NODE_LABEL_VERTICES_COUNT).numberInt() *
						bson.getField(REPO_NODE_LABEL_UV_CHANNELS_COUNT).numberInt();
					std::vector<aiVector2t<float>> * vec = new std::vector<aiVector2t<float>>();
					repo::core::RepoTranscoderBSON::retrieve(element, uvChannelsSize, vec);
					value = QString(repo::core::RepoTranscoderString::toString(*vec).c_str());
					type = QString("BSONBinDataGeneral");
					delete vec;
				}
				else if (REPO_NODE_LABEL_FACES == key)
				{
					std::vector<aiFace> * vec = new std::vector<aiFace>();
					repo::core::RepoNodeMesh::retrieveFacesArray(
						bson.getField(REPO_NODE_LABEL_FACES),
						bson.getField(REPO_NODE_LABEL_API).numberInt(),
						bson.getField(REPO_NODE_LABEL_FACES_BYTE_COUNT).numberInt(),
						bson.getField(REPO_NODE_LABEL_FACES_COUNT).numberInt(),
						vec);
					value = QString(repo::core::RepoTranscoderString::toString(*vec).c_str());
					type = QString("BSONBinDataGeneral");
					delete vec;
				}
				else 
				{
					value = QString::fromStdString(element.toString());
					type = QString("BSONBinData");
				}						
				break;	
			case mongo::Bool : 
				value = element.booleanSafe();
				type = QString("BSONBool");
				break;
			case mongo::Date : 				
				datetime.setMSecsSinceEpoch(element.Date());
				value = datetime;
				type = QString("BSONDate");				
				break;
			case mongo::jstOID : // ObjectId
				value = QString::fromStdString(element.OID().toString());
				type = QString("BSONObjID");
				break;
			case mongo::NumberDouble : // double precision floating point value
				value = element.numberDouble();
				type = QString("BSONDouble");
				break;	
			case mongo::NumberInt : // 32 bit signed integer 
				value = element.numberInt();
				type = QString("BSONInt32");
				break;
			case mongo::NumberLong : // 64 bit signed integer
				value = element.numberLong();
				type = QString("BSONInt64");
				break;
			case mongo::Object : // embedded object
				value = (unsigned long long) element.embeddedObject().objsize(); //nFields();
				type = QString("BSONObj");
				recursion = true;			
				break;
			case mongo::String : // character string, stored in utf8 
				value = QString::fromStdString(element.String());
				type = QString("BSONString");					
				break;	
			//case JSTypeMax : /** max type that is not MaxKey */
			//case MaxKey : // larger than all other types			
			//case Timestamp : /** Updated to a Date with value next OpTime on insert */
			//case Undefined : // Undefined type 
			//case jstNULL : // null type 
			//case RegEx : // regular expression, a pattern with options 
			//case DBRef : // deprecated / will be redesigned 
			//case Code : // deprecated / use CodeWScope 
			//case Symbol : // a programming language (e.g., Python) symbol 
			//case CodeWScope : // javascript code that can execute on the database server, with SavedContext 
			default :
				value = QString::fromStdString(element.toString());
				type = QString("Undefined");
				break;
		}

		emit keyValuePairAdded(key, value, type, depth);
		if (!cancelled && recursion)
			decodeRecords(element.embeddedObject(), depth + 1);
	}
}
