#include "CharacterLoader.h"
#include "Character.h"
#include "CharacterAction.h"
#include "CharacterController.h"
#include "Game.h"
#include "Brain.h"
#include "CsvReader.h"
#include "Sound.h"
#include "Camera.h"


using namespace std;


// csvファイルから読み込んだCHARACTER:ドメインからオブジェクトを作成
CharacterLoader::CharacterLoader() {
	m_focusId = -1;
	m_playerId = -1;
	m_playerCharacter_p = nullptr;
}

void CharacterLoader::addCharacter(map<string, string> dataMap) {
	// areaカラムがない場合は-1にマッピングしておく。
	int areaNum = -1;
	if (dataMap.find("area") != dataMap.end()) {
		areaNum = stoi(dataMap["area"]);
	}
	m_characters[areaNum].push_back(dataMap);
}

// 特定のエリアの追加オブジェクトのvectorを取得
pair<vector<Character*>, vector<CharacterController*> > CharacterLoader::getCharacters(Camera* camera_p, SoundPlayer* soundPlayer_p, int areaNum) {

	pair<vector<Character*>, vector<CharacterController*> > res;

	for (unsigned int i = 0; i < m_characters[areaNum].size(); i++) {
		string name = m_characters[areaNum][i]["name"];
		int x = stoi(m_characters[areaNum][i]["x"]);
		int y = stoi(m_characters[areaNum][i]["y"]);
		bool sound = (bool)stoi(m_characters[areaNum][i]["sound"]);
		int groupId = stoi(m_characters[areaNum][i]["groupId"]);
		string actionName = m_characters[areaNum][i]["action"];
		string brainName = m_characters[areaNum][i]["brain"];
		string controllerName = m_characters[areaNum][i]["controller"];
		bool cameraFlag = (bool)stoi(m_characters[areaNum][i]["camera"]);
		bool playerFlag = (bool)stoi(m_characters[areaNum][i]["player"]);

		// キャラを作成
		Character* character = createCharacter(name.c_str(), 100, x, y, groupId);

		// HP設定（指定されている場合のみ）
		if (m_characters[areaNum][i].find("hp") != m_characters[areaNum][i].end()) {
			string s = m_characters[areaNum][i]["hp"];
			if (s == "0") { character->setHp(0); }
			else if (s == "max") { character->setHp(character->getMaxHp()); }
		}

		// カメラをセット
		if (cameraFlag && character != nullptr) {
			camera_p->setPoint(character->getCenterX(), character->getCenterY());
			m_focusId = character->getId();
		}

		// プレイヤーが操作中のキャラとしてセット
		if (playerFlag && m_playerId == -1 && character != nullptr) {
			m_playerId = character->getId();
			m_playerCharacter_p = character;
			m_playerCharacter_p->setLeftDirection(false); // プレイヤーだけ右向き
		}

		// アクションを作成
		SoundPlayer* soundPlayer = sound ? soundPlayer_p : nullptr;
		CharacterAction* action = createAction(actionName, character, soundPlayer);

		// Brainを作成
		Brain* brain = createBrain(brainName, camera_p);

		// コントローラを作成
		CharacterController* controller = nullptr;
		if (action != nullptr && brain != nullptr) {
			controller = createController(controllerName, brain, action);
		}

		// 完成したキャラとコントローラを保存
		if (character != nullptr && controller != nullptr) {
			res.first.push_back(character);
			res.second.push_back(controller);
		}
	}

	return res;
}

// キャラのエリアと座標をセーブする
void CharacterLoader::saveCharacterData(CharacterData* characterData) {
	for (auto it = m_characters.begin(); it != m_characters.end(); it++) {
		int areaNum = it->first;
		vector<map<string, string> > characters = it->second;
		for (unsigned int i = 0; i < characters.size(); i++) {
			if (characters[i]["name"] == characterData->name()) {
				characterData->setAreaNum(areaNum);
				characterData->setX(stoi(characters[i]["x"]));
				characterData->setY(stoi(characters[i]["y"]));
				characterData->setSoundFlag((bool)stoi(characters[i]["sound"]));
				characterData->setActionName(characters[i]["action"].c_str());
				characterData->setBrainName(characters[i]["brain"].c_str());
				characterData->setControllerName(characters[i]["controller"].c_str());
				if (characters[i].find("hp") != characters[i].end()) {
					string s = characters[i]["hp"];
					if (s == "0") { characterData->setHp(0); }
					else if (s == "max") { characterData->setHp(-1); }
				}
				return;
			}
		}
	}
}