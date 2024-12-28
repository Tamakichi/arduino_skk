//
// Arduino SKK 漢字変換サンプル
//

#include "arduino.h"
#include "JString.h"
#include "skk.h"

int16_t readline(char *text, uint8_t len) {
  char *ptr = text;
  uint8_t l = 0;

  while(true) {
    if (l >= len) 
      break;
    if (Serial.available() > 0) {
      char c = Serial.read();
      if (c == '\r' || c == '\f' || c == '\n') {
        Serial.println();
        break;
      }
      Serial.write(c);
      *ptr = c; ptr++; l++;
    }
  }
  *ptr = '\0';
  return l;
}

// JStringクラスライブラリテスト
void test_jstring() {
    const char *text = "🍺Åあ1い1うＡＡ🍺";
    char *src = (char *)text;
    char dst[64];
    Serial.print(F("text=")); Serial.println(text);
    Serial.print(F("test01 len=")); Serial.print(JString::len(text)); 
    Serial.print(F(" bytes=")); Serial.println(JString::bytes(text));

    for (uint16_t i=0; i< JString::len(text); i++) {    
      char dst[8];
      char cnv[8];
      uint16_t len = JString::get(dst, src);
      uint32_t code = JString::utf8to32(dst);
      uint16_t bytes = JString::utf32to8(cnv, code);
      Serial.print(F("at "));Serial.print(i);
      Serial.print(F(" c=\""));Serial.print(dst);Serial.print("\"");
      Serial.print(F(" len="));Serial.print(len);
      Serial.print(F(" code="));Serial.print(code,HEX);
      Serial.print(F(" cnv="));Serial.print(cnv);
      Serial.print(F(" bytes="));Serial.println(bytes);
      src+=len;
    }

    char* s = "karuikibinnakonekonanbikiiruka?";
    JString::roma_to_kana(dst, s);
    Serial.print(s);
    Serial.print(F(" => "));
    Serial.println(dst);
}

// SKKクラスライブラリテスト
void test_skk() {
  SKK  skk;
  char hiragana[32];
	char katakana[32];
  char okuri[32];
	char word[32];
	char in_text[32];
  uint32_t index;
  
  int32_t rc = skk.begin();
	Serial.print(F("辞書の登録ワード数: ")); Serial.println(rc);
  Serial.println(F("[変換テスト]"));
	while(true) {
		Serial.print(F("入力:"));
		readline(in_text, sizeof(in_text));
  	rc = skk.get_kouho_list_index(&index, okuri, in_text);
		if (rc > 0) {
			uint16_t cnt = skk.count_kouho_list_by_index(index);	
      Serial.print(F(" 候補 ")); Serial.print(cnt); Serial.print(F("件[ "));
      for (uint16_t i=0; i< cnt; i++) {
				rc = skk.get_kouho_by_index(word, i, index);
				Serial.print(i); Serial.print(":"); Serial.print(word);
			}
      Serial.print(F(" ] ")); Serial.print(okuri);
		} else {
			Serial.println(F("候補なし"));
		}
		
		int16_t rc1 = skk.roma_to_kana(hiragana, in_text);
		int16_t rc2 = skk.kana_to_katakana(katakana, hiragana);
		if (rc1 || rc2 ) {
			Serial.print(F(" | "));
			if (rc1)
        Serial.print(F("h:")); Serial.print(hiragana); 
			if (rc2)
        Serial.print(F(" k:")); Serial.print(katakana); 
		}
		Serial.println();

	}
  skk.end();
}

void setup() {
  Serial.begin(115200);
  
  // UTF8文字列支援ライブラリテスト  
  Serial.println(F("<UTF8文字列支援ライブラリ テスト>"));
  test_jstring();  

  // SKK漢字入力テスト
  Serial.println(F("<SKK漢字入力テスト>"));
  test_skk();
}

void loop() {
  // put your main code here, to run repeatedly:

}
