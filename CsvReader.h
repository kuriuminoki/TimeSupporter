#ifndef CSV_READER_H_INCLUDED
#define CSV_READER_H_INCLUDED

#include <vector>
#include <string>
#include <map>
#include <sstream>


int str2color(std::string colorName);


class CsvReader {
private:
	/*
	* データ
	* m_data[行番号]<カラム名, データ>
	*/
	std::vector<std::map<std::string, std::string> > m_data;

	/*
	* カラム名のリスト
	*/
	std::vector<std::string> m_columnNames;

public:
	// ファイル名を指定してCSVファイルを読み込む
	CsvReader(const char* fileName);

	/*
	* カラム名がvalueのデータを取得
	* 例：findOne("Name", "キャラ名");
	*/ 
	std::map<std::string, std::string> findOne(const char* columnName, const char* value);

	/*
	* 全データを返す
	*/
	std::vector<std::map<std::string, std::string> > getData() const;
};


/*
* DOMAIN対応版
* TODO: CsvReaderを使いまわしてリファクタリング
*/
class CsvReader2 {
private:
	/*
	* データ
	* m_data[行番号]<カラム名, データ>
	*/
	std::map<std::string, std::vector<std::map<std::string, std::string> > > m_data;

	/*
	* カラム名のリスト
	*/
	std::map<std::string, std::vector<std::string> > m_columnNames;

public:
	// ファイル名を指定してCSVファイルを読み込む
	CsvReader2(const char* fileName);

	std::vector<std::map<std::string, std::string> > getDomainData(const char* domainName) { return m_data[domainName]; }

};


class Character;
class CharacterController;
class Object;
class DoorObject;
class SoundPlayer;
class Camera;

class AreaReader {
private:
	// 移動元
	int m_fromAreaNum;

	// アクション作成する用
	SoundPlayer* m_soundPlayer_p;

	// カメラはこのクラスで作成する
	Camera* m_camera_p;

	// 今カメラで見ているキャラ
	int m_focusId;

	// プレイヤーが操作するキャラ
	int m_playerId;

	// BGM
	std::string m_bgmName;
	int m_bgmVolume;

	// キャラクター
	std::vector<Character*> m_characters;

	// コントローラ
	std::vector<CharacterController*> m_characterControllers;

	// オブジェクト
	std::vector<Object*> m_objects;

	// 扉オブジェクト
	std::vector<Object*> m_doorObjects;

	// 背景画像と色
	int m_backGroundGraph, m_backGroundColor;

	// プレイヤーのキャラのポインタ
	Character* m_playerCharacter_p;

public:
	AreaReader(int fromAreaNum, int toAreaNum, SoundPlayer* soundPlayer);

	inline Camera* getCamera() const { return m_camera_p; }

	inline int getFocusId() const { return m_focusId; }

	inline int getPlayerId() const { return m_playerId; }

	inline std::string getBgm() const { return m_bgmName; }

	inline int getBgmVolume() const { return m_bgmVolume; }

	inline std::vector<Character*> getCharacters() const { return m_characters; }

	inline std::vector<CharacterController*> getCharacterControllers() const { return m_characterControllers; }

	inline std::vector<Object*> getObjects() const { return m_objects; }

	inline std::vector<Object*> getDoorObjects() const { return m_doorObjects; }

	inline void getBackGround(int& graphHandle, int& colorHandle) const {
		graphHandle = m_backGroundGraph;
		colorHandle = m_backGroundColor;
	}

private:
	void loadBGM(std::map<std::string, std::string> dataMap);
	void loadBackGround(std::map<std::string, std::string> dataMap);
	void setFollow();
};

#endif