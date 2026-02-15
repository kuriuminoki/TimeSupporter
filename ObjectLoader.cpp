#include "ObjectLoader.h"
#include "Object.h"
#include "CsvReader.h"
#include "Game.h"
#include "Define.h"


using namespace std;


// csvファイルから読み込んだOBJECT:ドメインからオブジェクトを作成
ObjectLoader::ObjectLoader() {

}

void ObjectLoader::addObject(map<string, string> dataMap) {
	// areaカラムがない場合は-1にマッピングしておく。
	int areaNum = -1;
	if (dataMap.find("area") != dataMap.end()) {
		areaNum = stoi(dataMap["area"]);
	}
	m_objects[areaNum].push_back(dataMap);
}

// 特定のエリアの追加オブジェクトのvectorを取得 pair<Objectのリスト, Doorのリスト>
pair<vector<Object*>, vector<Object*> > ObjectLoader::getObjects(int areaNum) {

	pair<vector<Object*>, vector<Object*> > res;
	for (unsigned int i = 0; i < m_objects[areaNum].size(); i++) {
		string name = m_objects[areaNum][i]["name"];
		int x1 = stoi(m_objects[areaNum][i]["x1"]);
		int y1 = stoi(m_objects[areaNum][i]["y1"]);
		int x2 = stoi(m_objects[areaNum][i]["x2"]);
		int y2 = stoi(m_objects[areaNum][i]["y2"]);
		string graph = m_objects[areaNum][i]["graph"];
		string color = m_objects[areaNum][i]["color"];
		int hp = stoi(m_objects[areaNum][i]["hp"]);
		string other = m_objects[areaNum][i]["other"];

		int colorHandle = str2color(color);
		Object* object = nullptr;
		if (name == "Box") {
			object = new BoxObject(x1, y1, x2, y2, graph.c_str(), colorHandle, hp);
		}
		else if (name == "Triangle") {
			bool leftDown = false;
			if (other == "leftDown") { leftDown = true; }
			object = new TriangleObject(x1, y1, x2, y2, graph.c_str(), colorHandle, leftDown, hp);
		}
		if (object != nullptr) { res.first.push_back(object); }
		else if (name == "Area") {
			// 左
			res.first.push_back(new BoxObject(x1 - GAME_WIDE, y1 - GAME_HEIGHT, x1, y2, graph.c_str(), colorHandle, -1));
			// 右
			res.first.push_back(new BoxObject(x2, y1 - GAME_HEIGHT, x2 + GAME_WIDE, y2, graph.c_str(), colorHandle, -1));
			// 上
			res.first.push_back(new BoxObject(x1, y1 - GAME_HEIGHT, x2, y1, graph.c_str(), colorHandle, -1));
			// 下
			res.first.push_back(new BoxObject(x1 - GAME_WIDE, y2, x2 + GAME_WIDE, y2 + GAME_HEIGHT, graph.c_str(), colorHandle, -1));
		}

		// 扉オブジェクトは別に分ける
		if (name == "Door") {
			res.second.push_back(new DoorObject(x1, y1, x2, y2, graph.c_str(), stoi(other)));
		}
		else if (name == "Stage") {
			res.second.push_back(new StageObject(x1, y1, x2, y2, graph.c_str(), stoi(other)));
		}
	}
	
	return res;

}

// キャラのエリアと座標をセーブする
void ObjectLoader::saveDoorData(vector<DoorData*>& doorData) {
	unsigned int initSize = (unsigned int)doorData.size();
	for (auto it = m_objects.begin(); it != m_objects.end(); it++) {
		int areaNum = it->first;
		vector<map<string, string> > doors = it->second;
		for (unsigned int i = 0; i < doors.size(); i++) {
			if (doors[i]["name"] != "Door") {
				continue;
			}
			int x1 = stoi(m_objects[areaNum][i]["x1"]);
			int y1 = stoi(m_objects[areaNum][i]["y1"]);
			int x2 = stoi(m_objects[areaNum][i]["x2"]);
			int y2 = stoi(m_objects[areaNum][i]["y2"]);
			string graph = m_objects[areaNum][i]["graph"];
			int from = stoi(doors[i]["area"]);
			int to = stoi(doors[i]["other"]);
			bool exist = false;
			for (unsigned j = 0; j < initSize; j++) {
				// 既にセーブデータに存在するドア
				if (from == doorData[j]->from() && to == doorData[j]->to()) {
					exist = true;
					doorData[j]->setX1(x1);
					doorData[j]->setY1(y1);
					doorData[j]->setX2(x2);
					doorData[j]->setY2(y2);
					doorData[j]->setFileName(graph.c_str());
					break;
				}
			}
			// セーブデータに存在しないから新規追加するドア
			if (!exist) {
				doorData.push_back(new DoorData(x1, y1, x2, y2, from, to, graph.c_str()));
			}
		}
	}
	
}