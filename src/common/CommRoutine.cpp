//------------------------------------------------------------------------------
// CommRoutine.cpp
//------------------------------------------------------------------------------

#include <strsafe.h>

#include <string>

#include "CommRoutine.h"
#include "CaptionDef.h"

using namespace std;

static const char HiraTable[][3] = {
    "ぁ", "あ", "ぃ", "い", "ぅ", "う", "ぇ",
    "え", "ぉ", "お", "か", "が", "き", "ぎ", "く",
    "ぐ", "け", "げ", "こ", "ご", "さ", "ざ", "し",
    "じ", "す", "ず", "せ", "ぜ", "そ", "ぞ", "た",
    "だ", "ち", "ぢ", "っ", "つ", "づ", "て", "で",
    "と", "ど", "な", "に", "ぬ", "ね", "の", "は",
    "ば", "ぱ", "ひ", "び", "ぴ", "ふ", "ぶ", "ぷ",
    "へ", "べ", "ぺ", "ほ", "ぼ", "ぽ", "ま", "み",
    "む", "め", "も", "ゃ", "や", "ゅ", "ゆ", "ょ",
    "よ", "ら", "り", "る", "れ", "ろ", "ゎ", "わ",
    "ゐ", "ゑ", "を", "ん", "　", "　", "　", "ゝ",
    "ゞ", "ー", "。", "「", "」", "、", "・",
    "！", "”", "＃", "＄", "％", "＆", "’",
    "（", "）", "＊", "＋", "，", "−", "．", "／",
    "０", "１", "２", "３", "４", "５", "６", "７",
    "８", "９", "：", "；", "＜", "＝", "＞", "？",
    "＠", "Ａ", "Ｂ", "Ｃ", "Ｄ", "Ｅ", "Ｆ", "Ｇ",
    "Ｈ", "Ｉ", "Ｊ", "Ｋ", "Ｌ", "Ｍ", "Ｎ", "Ｏ",
    "Ｐ", "Ｑ", "Ｒ", "Ｓ", "Ｔ", "Ｕ", "Ｖ", "Ｗ",
    "Ｘ", "Ｙ", "Ｚ", "［", "￥", "］", "＾", "＿",
    "‘", "ａ", "ｂ", "ｃ", "ｄ", "ｅ", "ｆ", "ｇ",
    "ｈ", "ｉ", "ｊ", "ｋ", "ｌ", "ｍ", "ｎ", "ｏ",
    "ｐ", "ｑ", "ｒ", "ｓ", "ｔ", "ｕ", "ｖ", "ｗ",
    "ｘ", "ｙ", "ｚ", "｛", "｜", "｝", "￣"
};

static const char HalfHiraTable[][3] = {
    "ぁ", "あ", "ぃ", "い", "ぅ", "う", "ぇ",
    "え", "ぉ", "お", "か", "が", "き", "ぎ", "く",
    "ぐ", "け", "げ", "こ", "ご", "さ", "ざ", "し",
    "じ", "す", "ず", "せ", "ぜ", "そ", "ぞ", "た",
    "だ", "ち", "ぢ", "っ", "つ", "づ", "て", "で",
    "と", "ど", "な", "に", "ぬ", "ね", "の", "は",
    "ば", "ぱ", "ひ", "び", "ぴ", "ふ", "ぶ", "ぷ",
    "へ", "べ", "ぺ", "ほ", "ぼ", "ぽ", "ま", "み",
    "む", "め", "も", "ゃ", "や", "ゅ", "ゆ", "ょ",
    "よ", "ら", "り", "る", "れ", "ろ", "ゎ", "わ",
    "ゐ", "ゑ", "を", "ん", " ", " ", " ", "ゝ",
    "ゞ", "ｰ", "｡", "｢", "｣", "､", "･",
    "!", "\"", "#", "$", "%", "&", "'",
    "(", ")", "*", "+", ", ", "-", ".", "/",
    "0", "1", "2", "3", "4", "5", "6", "7",
    "8", "9", ":", ";", "<", "=", ">", "?",
    "@", "A", "B", "C", "D", "E", "F", "G",
    "H", "I", "J", "K", "L", "M", "N", "O",
    "P", "Q", "R", "S", "T", "U", "V", "W",
    "X", "Y", "Z", "[", "￥", "]", "^", "_",
    "`", "a", "b", "c", "d", "e", "f", "g",
    "h", "i", "j", "k", "l", "m", "n", "o",
    "p", "q", "r", "s", "t", "u", "v", "w",
    "x", "y", "z", "{", "|", "}", "￣"
};

static const char KanaTable[][3] = {
    "ァ", "ア", "ィ", "イ", "ゥ", "ウ", "ェ",
    "エ", "ォ", "オ", "カ", "ガ", "キ", "ギ", "ク",
    "グ", "ケ", "ゲ", "コ", "ゴ", "サ", "ザ", "シ",
    "ジ", "ス", "ズ", "セ", "ゼ", "ソ", "ゾ", "タ",
    "ダ", "チ", "ヂ", "ッ", "ツ", "ヅ", "テ", "デ",
    "ト", "ド", "ナ", "ニ", "ヌ", "ネ", "ノ", "ハ",
    "バ", "パ", "ヒ", "ビ", "ピ", "フ", "ブ", "プ",
    "ヘ", "ベ", "ペ", "ホ", "ボ", "ポ", "マ", "ミ",
    "ム", "メ", "モ", "ャ", "ヤ", "ュ", "ユ", "ョ",
    "ヨ", "ラ", "リ", "ル", "レ", "ロ", "ヮ", "ワ",
    "ヰ", "ヱ", "ヲ", "ン", "ヴ", "ヵ", "ヶ", "ヽ",
    "ヾ", "ー", "。", "「", "」", "、", "・"
};

static const char HalfKanaTable[][3] = {
    "ｧ", "ｱ", "ｨ", "ｲ", "ｩ", "ｳ", "ｪ",
    "ｴ", "ｫ", "ｵ", "ｶ", "ｶﾞ", "ｷ", "ｷﾞ", "ｸ",
    "ｸﾞ", "ｹ", "ｹﾞ", "ｺ", "ｺﾞ", "ｻ", "ｻﾞ", "ｼ",
    "ｼﾞ", "ｽ", "ｽﾞ", "ｾ", "ｾﾞ", "ｿ", "ｿﾞ", "ﾀ",
    "ﾀﾞ", "ﾁ", "ﾁﾞ", "ｯ", "ﾂ", "ﾂﾞ", "ﾃ", "ﾃﾞ",
    "ﾄ", "ﾄﾞ", "ﾅ", "ﾆ", "ﾇ", "ﾈ", "ﾉ", "ﾊ",
    "ﾊﾞ", "ﾊﾟ", "ﾋ", "ﾋﾞ", "ﾋﾟ", "ﾌ", "ﾌﾞ", "ﾌﾟ",
    "ﾍ", "ﾍﾞ", "ﾍﾟ", "ﾎ", "ﾎﾞ", "ﾎﾟ", "ﾏ", "ﾐ",
    "ﾑ", "ﾒ", "ﾓ", "ｬ", "ﾔ", "ｭ", "ﾕ", "ｮ",
    "ﾖ", "ﾗ", "ﾘ", "ﾙ", "ﾚ", "ﾛ", "ヮ", "ﾜ",
    "ヰ", "ヱ", "ｦ", "ﾝ", "ｳﾞ", "ヵ", "ヶ", "ヽ",
    "ヾ", "ｰ", "｡", "｢", "｣", "､", "･"
};

extern std::string GetHalfChar(std::string key)
{
//  std::string ret;
    CHAR ret[STRING_BUFFER_SIZE] = { 0 };
    BOOL bMatch = FALSE;

    // マッチしない文字は、そのまま使用
    const char *_p = key.c_str();
//  const char *_pret = ret.c_str();
    char *p = (char *)key.c_str();
//  char *pret = (char *)ret.c_str();

    while (p < _p + key.size()) {
        for (int i = 0; i < sizeof(HiraTable) / sizeof(HiraTable[0]) && p < _p + key.size(); i++) {
            bMatch = FALSE;
            if (memcmp(p, HiraTable[i], 2) == 0) {
//              ret += HalfHiraTable[i];
                strcat_s( ret, STRING_BUFFER_SIZE, HalfHiraTable[i] );
                p += 2;
                bMatch = TRUE;
                i = -1;
            }
        }

        for (int i = 0; i < sizeof(KanaTable) / sizeof(KanaTable[0]) && p < _p + key.size(); i++) {
            bMatch = FALSE;
            if (memcmp(p, KanaTable[i], 2) == 0) {
//              ret += HalfKanaTable[i];
                strcat_s(ret, STRING_BUFFER_SIZE, HalfKanaTable[i]);
                p += 2;
                bMatch = TRUE;
                i = -1;
            }
        }

        if (p < _p + key.size()) {
//          ret += strndup(p, 2);
            strncat_s(ret, STRING_BUFFER_SIZE, p, 2);
            p += 2;
        }
    }

    return ret;
}
