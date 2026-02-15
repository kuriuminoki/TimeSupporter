#include "CsvReader.h"
#include "Character.h"
#include "CharacterAction.h"
#include "CharacterController.h"
#include "Object.h"
#include "Camera.h"
#include "Brain.h"
#include "CharacterLoader.h"
#include "ObjectLoader.h"
#include "Define.h"
#include "DxLib.h"


using namespace std;


// 色の名前から色ハンドルを取得
int str2color(string colorName) {
	if (colorName == "white") { return WHITE; }
	else if (colorName == "gray") { return GRAY; }
	else if (colorName == "lightBlue") { return LIGHT_BLUE; }
	return -1;
}


// 一行分のテキストから一行分のデータに変換
vector<string> csv2vector(string buff) {
	vector<string> data;
	string oneCell = "";

	// 1文字ずつ見ていく
	for (int i = 0; buff[i] != '\0'; i++) {
		// CSVファイルなのでカンマで区切ってoneDataにpush_back
		if (buff[i] == ',') {
			data.push_back(oneCell);
			oneCell = "";
		}
		else { // カンマ以外の文字なら合体
			oneCell += buff[i];
		}
	}
	// 最後のセルにはカンマがない
	data.push_back(oneCell);

	return data;
}


// ファイル名を指定してCSVファイルを読み込む
CsvReader::CsvReader(const char* fileName) {

	// ファイルポインタ
	int fp;

	// バッファ
	const int size = 512;
	char buff[size];

	// ファイルを開く
	fp = FileRead_open(fileName);

	// バッファに一行目（カラム名）のテキストを入れる
	FileRead_gets(buff, size, fp);

	// カラム名のリストを取得
	m_columnNames = csv2vector(buff);

	// ファイルの終端までループ
	while (FileRead_eof(fp) == 0) {
		// いったんバッファに一行分のテキストを入れる
		FileRead_gets(buff, size, fp);

		// 一行分のテキストをデータにしてVectorに変換
		vector<string> oneDataVector;
		oneDataVector = csv2vector(buff);

		// vectorとカラム名を使ってmapを生成
		map<string, string> oneData;
		for (int i = 0; i < oneDataVector.size(); i++) {
			oneData[m_columnNames[i]] = oneDataVector[i];
		}

		// 一行分のmapデータをpush_back
		m_data.push_back(oneData);
	}

	// ファイルを閉じる
	FileRead_close(fp);
}


/*
* カラム名がvalueのデータを取得
* 例：findOne("name", "キャラ名");
* 見つからなかったら空のデータを返す
*/
map<string, string> CsvReader::findOne(const char* columnName, const char* value) {

	// m_data[i][columnName] == valueとなるiを調べる
	map<string, string>::iterator ite;
	for (int i = 0; i < m_data.size(); i++) {

		// カラム名に対応する値を取得
		ite = m_data[i].find(columnName);

		// そのカラム名が存在しない
		if (ite == m_data[i].end()) { continue; }

		// 条件に一致
		if (ite->second == value) {
			return m_data[i];
		}
	}

	// 空のデータを返す
	map<string, string> res;
	return res;
}


/*
* 全データを返す
*/
vector<map<string, string>> CsvReader::getData() const {
	return m_data;
}


// ファイル名を指定してCSVファイルを読み込む
CsvReader2::CsvReader2(const char* fileName) {

	// ファイルポインタ
	int fp;

	// バッファ
	const int size = 512;
	char buff[size];

	// ファイルを開く
	fp = FileRead_open(fileName);

	string domainName = "";
	// ファイルの終端までループ
	while (FileRead_eof(fp) == 0) {
		// いったんバッファに一行分のテキストを入れる
		FileRead_gets(buff, size, fp);

		// 一行分のテキストをデータにしてVectorに変換
		vector<string> oneDataVector = csv2vector(buff);

		if ((oneDataVector[1] == "" && oneDataVector[0].find(":") != std::string::npos) || oneDataVector[0] == "") {
			domainName = oneDataVector[0];
			FileRead_gets(buff, size, fp);
			m_columnNames[domainName] = csv2vector(buff);
		}
		else {
			// vectorとカラム名を使ってmapを生成
			map<string, string> oneData;
			for (int i = 0; i < oneDataVector.size(); i++) {
				oneData[m_columnNames[domainName][i]] = oneDataVector[i];
			}

			// 一行分のmapデータをpush_back
			m_data[domainName].push_back(oneData);
		}
	}

	// ファイルを閉じる
	FileRead_close(fp);
}



/*
* area/area?.csvからキャラクターやオブジェクトをロードする
* Character等をnewするため、このクラスをnewした後はgetして削除すること。
* このクラスでnewされたCharacter等はこのクラスで削除しない。
*/
AreaReader::AreaReader(int fromAreaNum, int toAreaNum, SoundPlayer* soundPlayer) {
	m_fromAreaNum = fromAreaNum;
	m_soundPlayer_p = soundPlayer;

	m_camera_p = nullptr;
	m_focusId = -1;
	m_playerId = -1;
	m_backGroundGraph = -1;
	m_backGroundColor = -1;
	m_bgmName = "";

	// ファイルを開く
	ostringstream fileName;
	fileName << "data/area/area" << toAreaNum << ".csv";

	CsvReader2 csvReader2(fileName.str().c_str());

	vector<map<string, string> > data;
	// BGM
	data = csvReader2.getDomainData("BGM:");
	for (unsigned int i = 0; i < data.size(); i++) {
		loadBGM(data[i]);
	}
	// CHARACTER
	data = csvReader2.getDomainData("CHARACTER:");
	CharacterLoader characterLoader;
	m_camera_p = new Camera(0, 0, 1.0);
	for (unsigned int i = 0; i < data.size(); i++) {
		characterLoader.addCharacter(data[i]);
	}
	pair<vector<Character*>, vector<CharacterController*> > cp = characterLoader.getCharacters(m_camera_p, soundPlayer);
	m_characters = cp.first;
	m_characterControllers = cp.second;
	m_focusId = characterLoader.getFocusId();
	m_playerId = characterLoader.getPlayerId();
	m_playerCharacter_p = characterLoader.getPlayerCharacter();
	// OBJECT
	data = csvReader2.getDomainData("OBJECT:");
	ObjectLoader objectLoader;
	for (unsigned int i = 0; i < data.size(); i++) {
		objectLoader.addObject(data[i]);
	}
	pair<vector<Object*>, vector<Object*> > op = objectLoader.getObjects();
	m_objects = op.first;
	m_doorObjects = op.second;
	// BACKGROUND
	data = csvReader2.getDomainData("BACKGROUND:");
	for (unsigned int i = 0; i < data.size(); i++) {
		loadBackGround(data[i]);
	}

	// 追跡対象をプレイヤーにする
	setFollow();
}

// BGMのロード
void AreaReader::loadBGM(std::map<std::string, std::string> dataMap) {
	ostringstream filePath;
	filePath << "sound/bgm/" << dataMap["name"];
	m_bgmName = filePath.str().c_str();
	m_bgmVolume = stoi(dataMap["volume"]);
}

// 背景のロード
void AreaReader::loadBackGround(std::map<std::string, std::string> dataMap) {
	string graphName = dataMap["graph"];
	string color = dataMap["color"];

	// 背景画像
	if (graphName != "null") {
		ostringstream filePath;
		filePath << "picture/backGround/" << graphName;
		m_backGroundGraph = LoadGraph(filePath.str().c_str());
	}
	else {
		m_backGroundGraph = -1;
	}
	// 背景色
	m_backGroundColor = str2color(color);
}

// follow対象をプレイヤーにする
void AreaReader::setFollow() {
	for (int i = 0; i < m_characterControllers.size(); i++) {
		if (m_characterControllers[i]->getBrain()->needSearchFollow()) {
			m_characterControllers[i]->searchFollowCandidate(m_playerCharacter_p);
		}
	}
}