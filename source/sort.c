#include <gccore.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include "menu.h"
#include "xml.h"
#include "cfg.h"
#include "video.h"
#include "gui.h"
#include "libwbfs/libwbfs.h"
#include "wbfs_fat.h"
#include "sort.h"
#include "wpad.h"
#include "gettext.h"
#include "gc.h"
#include "disc.h"

extern struct discHdr *all_gameList;
extern struct discHdr *gameList;
extern struct discHdr *filter_gameList;
extern bool enable_favorite;
extern s32 filter_gameCnt;

s32 filter_index = -1;
s32 filter_type = -1;
s32 sort_index = -1;
bool sort_desc = 0;

s32 default_sort_index = 0;
bool default_sort_desc = 0;
s32 default_filter_type = -1;

HashMap install_time;

s32 __sort_play_date(struct discHdr * hdr1, struct discHdr * hdr2, bool desc);
s32 __sort_install_date(struct discHdr * hdr1, struct discHdr * hdr2, bool desc);
s32 __sort_title(struct discHdr * hdr1, struct discHdr * hdr2, bool desc);
s32 __sort_releasedate(struct discHdr * hdr1, struct discHdr * hdr2, bool desc);
s32 __sort_players(struct discHdr * hdr1, struct discHdr * hdr2, bool desc);
s32 __sort_wifiplayers(struct discHdr * hdr1, struct discHdr * hdr2, bool desc);
s32 __sort_pub(struct discHdr * hdr1, struct discHdr * hdr2, bool desc);
s32 __sort_dev(struct discHdr * hdr1, struct discHdr * hdr2, bool desc);
s32 __sort_players_asc(const void *a, const void *b);
s32 __sort_players_desc(const void *a, const void *b);
s32 __sort_wifiplayers_asc(const void *a, const void *b);
s32 __sort_wifiplayers_desc(const void *a, const void *b);
s32 __sort_pub_asc(const void *a, const void *b);
s32 __sort_pub_desc(const void *a, const void *b);
s32 __sort_dev_asc(const void *a, const void *b);
s32 __sort_dev_desc(const void *a, const void *b);
s32 __sort_releasedate_asc(const void *a, const void *b);
s32 __sort_releasedate_desc(const void *a, const void *b);
s32 __sort_title_asc(const void *a, const void *b);
s32 __sort_title_desc(const void *a, const void *b);
s32 __sort_play_count_asc(const void *a, const void *b);
s32 __sort_play_count_desc(const void *a, const void *b);
s32 __sort_install_date_asc(const void *a, const void *b);
s32 __sort_install_date_desc(const void *a, const void *b);
s32 __sort_play_date_asc(const void *a, const void *b);
s32 __sort_play_date_desc(const void *a, const void *b);


char *featureTypes[featureCnt][2] =
{
	{ "online",     gts("Online Content") },
	{ "download",   gts("Downloadable Content") },
	{ "score",      gts("Online Score List") },
	{ "nintendods", gts("Nintendo DS Connectivity") },
};

char *accessoryTypes[accessoryCnt][2] =
{
	{ "wiimote",           gts("Wiimote") },
	{ "nunchuk",           gts("Nunchuk") },
	{ "motionplus",        gts("Motion+") },
	{ "gamecube",          gts("Gamecube") },
	{ "nintendods",        gts("Nintendo DS") },
	{ "classiccontroller", gts("Classic Controller") },
	{ "wheel",             gts("Wheel") },
	{ "zapper",            gts("Zapper") },
	{ "balanceboard",      gts("Balance Board") },
	{ "wiispeak",          gts("Wii Speak") },
	{ "microphone",        gts("Microphone") },
	{ "guitar",            gts("Guitar") },
	{ "drums",             gts("Drums") },
	{ "dancepad",          gts("Dance Pad") },
	{ "keyboard",          gts("Keyboard") },
	{ "vitalitysensor",    gts("Vitality Sensor") },
	{ "udraw",             gts("uDraw GameTablet") },
};

char *genreTypes[genreCnt][2];
struct Sorts sortTypes[sortCnt];

void build_arrays() {
	genreTypes[0][0] = "action";
	genreTypes[0][1] = (char *)gt("Action");
	genreTypes[1][0] = "adventure";
	genreTypes[1][1] = (char *)gt("Adventure");
	genreTypes[2][0] = "fighting";
	genreTypes[2][1] = (char *)gt("Fighting");
	genreTypes[3][0] = "music";
	genreTypes[3][1] = (char *)gt("Music");
	genreTypes[4][0] = "party";
	genreTypes[4][1] = (char *)gt("Party");
	genreTypes[5][0] = "platformer";
	genreTypes[5][1] = (char *)gt("Platformer");
	genreTypes[6][0] = "puzzle";
	genreTypes[6][1] = (char *)gt("Puzzle");
	genreTypes[7][0] = "racing";
	genreTypes[7][1] = (char *)gt("Racing");
	genreTypes[8][0] = "role-playing";
	genreTypes[8][1] = (char *)gt("RPG");
	genreTypes[9][0] = "simulation";
	genreTypes[9][1] = (char *)gt("Simulation");
	genreTypes[10][0] = "sport";
	genreTypes[10][1] = (char *)gt("Sports");
	genreTypes[11][0] = "strategy";
	genreTypes[11][1] = (char *)gt("Strategy");
	genreTypes[12][0] = "first-person shooter";
	genreTypes[12][1] = (char *)gt("1st-Person Shooter");
	genreTypes[13][0] = "arcade";
	genreTypes[13][1] = (char *)gt("Arcade");
	genreTypes[14][0] = "baseball";
	genreTypes[14][1] = (char *)gt("Baseball");
	genreTypes[15][0] = "basketball";
	genreTypes[15][1] = (char *)gt("Basketball");
	genreTypes[16][0] = "bike racing";
	genreTypes[16][1] = (char *)gt("Bike Racing");
	genreTypes[17][0] = "billiards";
	genreTypes[17][1] = (char *)gt("Billiards");
	genreTypes[18][0] = "board game";
	genreTypes[18][1] = (char *)gt("Board Game");
	genreTypes[19][0] = "bowling";
	genreTypes[19][1] = (char *)gt("Bowling");
	genreTypes[20][0] = "boxing";
	genreTypes[20][1] = (char *)gt("Boxing");
	genreTypes[21][0] = "business simulation";
	genreTypes[21][1] = (char *)gt("Business Sim");
	genreTypes[22][0] = "cards";
	genreTypes[22][1] = (char *)gt("Cards");
	genreTypes[23][0] = "chess";
	genreTypes[23][1] = (char *)gt("Chess");
	genreTypes[24][0] = "coaching";
	genreTypes[24][1] = (char *)gt("Coaching");
	genreTypes[25][0] = "compilation";
	genreTypes[25][1] = (char *)gt("Compilation");
	genreTypes[26][0] = "construction simulation";
	genreTypes[26][1] = (char *)gt("Construction Sim");
	genreTypes[27][0] = "cooking";
	genreTypes[27][1] = (char *)gt("Cooking");
	genreTypes[28][0] = "cricket";
	genreTypes[28][1] = (char *)gt("Cricket");
	genreTypes[29][0] = "dance";
	genreTypes[29][1] = (char *)gt("Dance");
	genreTypes[30][0] = "darts";
	genreTypes[30][1] = (char *)gt("Darts");
	genreTypes[31][0] = "drawing";
	genreTypes[31][1] = (char *)gt("Drawing");
	genreTypes[32][0] = "educational";
	genreTypes[32][1] = (char *)gt("Educational");
	genreTypes[33][0] = "exercise";
	genreTypes[33][1] = (char *)gt("Exercise");
	genreTypes[34][0] = "fishing";
	genreTypes[34][1] = (char *)gt("Fishing");
	genreTypes[35][0] = "fitness";
	genreTypes[35][1] = (char *)gt("Fitness");
	genreTypes[36][0] = "flight simulation";
	genreTypes[36][1] = (char *)gt("Flight Sim");
	genreTypes[37][0] = "football";
	genreTypes[37][1] = (char *)gt("Football");
	genreTypes[38][0] = "futuristic racing";
	genreTypes[38][1] = (char *)gt("Futuristic Racing");
	genreTypes[39][0] = "golf";
	genreTypes[39][1] = (char *)gt("Golf");
	genreTypes[40][0] = "health";
	genreTypes[40][1] = (char *)gt("Health");
	genreTypes[41][0] = "hockey";
	genreTypes[41][1] = (char *)gt("Hockey");
	genreTypes[42][0] = "hunting";
	genreTypes[42][1] = (char *)gt("Hunting");
	genreTypes[43][0] = "karaoke";
	genreTypes[43][1] = (char *)gt("Karaoke");
	genreTypes[44][0] = "kart racing";
	genreTypes[44][1] = (char *)gt("Kart Racing");
	genreTypes[45][0] = "life simulation";
	genreTypes[45][1] = (char *)gt("Life Simulation");
	genreTypes[46][0] = "management simulation";
	genreTypes[46][1] = (char *)gt("Management Sim");
	genreTypes[47][0] = "martial arts";
	genreTypes[47][1] = (char *)gt("Martial Arts");
	genreTypes[48][0] = "motorcycle racing";
	genreTypes[48][1] = (char *)gt("Motorcycle Racing");
	genreTypes[49][0] = "off-road racing";
	genreTypes[49][1] = (char *)gt("Off-Road Racing");
	genreTypes[50][0] = "p�tanque";
	genreTypes[50][1] = (char *)gt("Petanque");
	genreTypes[51][0] = "pinball";
	genreTypes[51][1] = (char *)gt("Pinball");
	genreTypes[52][0] = "poker";
	genreTypes[52][1] = (char *)gt("Poker");
	genreTypes[53][0] = "rhythm";
	genreTypes[53][1] = (char *)gt("Rhythm");
	genreTypes[54][0] = "rugby";
	genreTypes[54][1] = (char *)gt("Rugby");
	genreTypes[55][0] = "shooter";
	genreTypes[55][1] = (char *)gt("Shooter");
	genreTypes[56][0] = "sim racing";
	genreTypes[56][1] = (char *)gt("Sim Racing");
	genreTypes[57][0] = "skateboarding";
	genreTypes[57][1] = (char *)gt("Skateboarding");
	genreTypes[58][0] = "ski";
	genreTypes[58][1] = (char *)gt("Skiing");
	genreTypes[59][0] = "snowboarding";
	genreTypes[59][1] = (char *)gt("Snowboarding");
	genreTypes[60][0] = "soccer";
	genreTypes[60][1] = (char *)gt("Soccer");
	genreTypes[61][0] = "surfing";
	genreTypes[61][1] = (char *)gt("Surfing");
	genreTypes[62][0] = "survival horror";
	genreTypes[62][1] = (char *)gt("Survival Horror");
	genreTypes[63][0] = "table tennis";
	genreTypes[63][1] = (char *)gt("Table Tennis");
	genreTypes[64][0] = "tennis";
	genreTypes[64][1] = (char *)gt("Tennis");
	genreTypes[65][0] = "train simulation";
	genreTypes[65][1] = (char *)gt("Train Simulation");
	genreTypes[66][0] = "trivia";
	genreTypes[66][1] = (char *)gt("Trivia");
	genreTypes[67][0] = "truck racing";
	genreTypes[67][1] = (char *)gt("Truck Racing");
	genreTypes[68][0] = "virtual pet";
	genreTypes[68][1] = (char *)gt("Virtual Pet");
	genreTypes[69][0] = "volleyball";
	genreTypes[69][1] = (char *)gt("Volleyball");
	genreTypes[70][0] = "watercraft racing";
	genreTypes[70][1] = (char *)gt("Watercraft Racing");
	genreTypes[71][0] = "wrestling";
	genreTypes[71][1] = (char *)gt("Wrestling");



	strcpy(sortTypes[0].cfg_val, "title");
	strcpy(sortTypes[0].name, gt("Title"));
	sortTypes[0].sortAsc = __sort_title_asc;
	sortTypes[0].sortDsc = __sort_title_desc;
	strcpy(sortTypes[1].cfg_val, "players");
	strcpy(sortTypes[1].name, gt("Number of Players"));
	sortTypes[1].sortAsc = __sort_players_asc;
	sortTypes[1].sortDsc = __sort_players_desc;
	strcpy(sortTypes[2].cfg_val, "online_players");
	strcpy(sortTypes[2].name, gt("Number of Online Players"));
	sortTypes[2].sortAsc = __sort_wifiplayers_asc;
	sortTypes[2].sortDsc = __sort_wifiplayers_desc;
	strcpy(sortTypes[3].cfg_val, "publisher");
	strcpy(sortTypes[3].name, gt("Publisher"));
	sortTypes[3].sortAsc = __sort_pub_asc;
	sortTypes[3].sortDsc = __sort_pub_desc;
	strcpy(sortTypes[4].cfg_val, "developer");
	strcpy(sortTypes[4].name, gt("Developer"));
	sortTypes[4].sortAsc = __sort_dev_asc;
	sortTypes[4].sortDsc = __sort_dev_desc;
	strcpy(sortTypes[5].cfg_val, "release");
	strcpy(sortTypes[5].name, gt("Release Date"));
	sortTypes[5].sortAsc = __sort_releasedate_asc;
	sortTypes[5].sortDsc = __sort_releasedate_desc;
	strcpy(sortTypes[6].cfg_val, "play_count");
	strcpy(sortTypes[6].name, gt("Play Count"));
	sortTypes[6].sortAsc = __sort_play_count_asc;
	sortTypes[6].sortDsc = __sort_play_count_desc;
	strcpy(sortTypes[7].cfg_val, "play_date");
	strcpy(sortTypes[7].name, gt("Last Play Date"));
	sortTypes[7].sortAsc = __sort_play_date_asc;
	sortTypes[7].sortDsc = __sort_play_date_desc;
	strcpy(sortTypes[8].cfg_val, "install");
	strcpy(sortTypes[8].name, gt("Install Date"));
	sortTypes[8].sortAsc = __sort_install_date_asc;
	sortTypes[8].sortDsc = __sort_install_date_desc;

}

int get_accesory_id(char *accessory)
{
	int i;
	for (i=0; i<accessoryCnt; i++) {
		if (strcmp(accessory, accessoryTypes[i][0]) == 0) return i;
	}
	return -1;
}

const char* get_accesory_name(int i)
{
	if (i<accessoryCnt) {
		return gt(accessoryTypes[i][1]);
	}
	return NULL;
}

int get_feature_id(char *feature)
{
	int i;
	for (i=0; i<featureCnt; i++) {
		if (strcmp(feature, featureTypes[i][0]) == 0) return i;
	}
	return -1;
}

void reset_sort_default()
{
	sort_index = default_sort_index;
	sort_desc = default_sort_desc;
	filter_type = default_filter_type;
}

void __set_default_sort()
{

	build_arrays();
	char tmp[20];
	bool dsc = 0;
	int n = 0;
	strncpy(tmp, CFG.sort, sizeof(tmp));
	char * ptr = strchr(tmp, '-');
	if (ptr) {
		*ptr = '\0';
		dsc = (*(ptr+1) == 'd' || *(ptr+1) == 'D');
	}
	for (; n<sortCnt; n++) {
		if (!strncmp(sortTypes[n].cfg_val, tmp, strlen(tmp))) {
			sort_index = default_sort_index = n;
			if (!dsc) {
				default_sort_function = (*sortTypes[sort_index].sortAsc);
				sort_desc = default_sort_desc = 0;
			} else {
				default_sort_function = (*sortTypes[sort_index].sortDsc);
				sort_desc = default_sort_desc = 1;
			}
			return;
		}
	}
	sort_index = default_sort_index = 0;
	default_sort_function =  (*sortTypes[sort_index].sortAsc);
}

int filter_features(struct discHdr *list, int cnt, char *feature, bool requiredOnly)
{
	int i;
	for (i=0; i<cnt;) {
		if (!hasFeature(feature, list[i].id)) {
			memcpy(list+i, list+i+1, (cnt-i-1) * sizeof(struct discHdr));
			cnt--;
		} else {
			i++;
		}
	}
	return cnt;
}

int filter_online(struct discHdr *list, int cnt, char * name, bool notused)
{
	int i;
	for (i=0; i<cnt;) {
		gameXMLinfo *g = get_game_info_id(list[i].id);
		if (!g || g->wifiplayers < 1) {
			memcpy(list+i, list+i+1, (cnt-i-1) * sizeof(struct discHdr));
			cnt--;
		} else {
			i++;
		}
	}
	return cnt;
}

int filter_controller(struct discHdr *list, int cnt, char *controller, bool requiredOnly)
{
	int i;
	for (i=0; i<cnt;) {
		if (getControllerTypes(controller, list[i].id) < (requiredOnly ? 1 : 0)) {
			memcpy(list+i, list+i+1, (cnt-i-1) * sizeof(struct discHdr));
			cnt--;
		} else {
			i++;
		}
	}
	return cnt;
}

int filter_genre(struct discHdr *list, int cnt, char *genre, bool notused)
{
	int i;
	for (i=0; i<cnt;) {
		if (!hasGenre(genre, list[i].id)) {
			memcpy(list+i, list+i+1, (cnt-i-1) * sizeof(struct discHdr));
			cnt--;
		} else {
			i++;
		}
	}
	return cnt;
}

int filter_gamecube(struct discHdr *list, int cnt, char *ignore, bool notused)
{
	int i;
	for (i=0; i<cnt;) 
	{
		if (list[i].magic == GC_GAME_ON_DRIVE) 
		{
			i++;
		} 
		else 
		{
			memcpy(list+i, list+i+1, (cnt-i-1) * sizeof(struct discHdr));
			cnt--;
		}
	}
	return cnt;
}

int filter_wii(struct discHdr *list, int cnt, char *ignore, bool notused)
{
	int i;
	for (i=0; i<cnt;) 
	{
		if (list[i].magic == WII_MAGIC) 
		{
			i++;
		} 
		else 
		{
			memcpy(list+i, list+i+1, (cnt-i-1) * sizeof(struct discHdr));
			cnt--;
		}
	}
	return cnt;
}

int filter_channel(struct discHdr *list, int cnt, char *ignore, bool notused)
{
	int i;
	for (i=0; i<cnt;) 
	{
		if (list[i].magic == CHANNEL_MAGIC) 
		{
			i++;
		} 
		else 
		{
			memcpy(list+i, list+i+1, (cnt-i-1) * sizeof(struct discHdr));
			cnt--;
		}
	}
	return cnt;
}

int filter_play_count(struct discHdr *list, int cnt, char *ignore, bool notused)
{
	int i;
	for (i=0; i<cnt;) {
		if (getPlayCount(list[i].id) > 0) {
			memcpy(list+i, list+i+1, (cnt-i-1) * sizeof(struct discHdr));
			cnt--;
		} else {
			i++;
		}
	}
	return cnt;
}

int filter_games(int (*filter) (struct discHdr *, int, char *, bool), char * name, bool num)
{
	int i, len;
	int ret = 0;
	u8 *id = NULL;
	// filter
	if (filter_gameList) {
		len = sizeof(struct discHdr) * all_gameCnt;
		memcpy(filter_gameList, all_gameList, len);
		filter_gameCnt = filter(filter_gameList, all_gameCnt, name, num);
	}
	if (gameSelected >= 0 && gameSelected < gameCnt) {
		id = gameList[gameSelected].id;
	}
	//if (filter_gameCnt > 0) {
		gameList = filter_gameList;
		gameCnt = filter_gameCnt;
		ret = 1;
	/*} else {
		// this won't work because gameList might
		// already point to filter_gameCnt
		Con_Clear();
		FgColor(CFG.color_header);
		printf("\n");
		printf(" %s ", CFG.cursor);
		printf("%s", gt("No games found!"));
		printf("\n");
		printf(gt("Loading previous game list..."));
		printf("\n");
		DefaultColor();
		__console_flush(0);
		sleep(1);
		ret = -1;
	}
	*/
	gameStart = 0;
	gameSelected = 0;
	if (id) for (i=0; i<gameCnt; i++) {
		if (strncmp((char*)gameList[i].id, (char*)id, 6) == 0) {
			gameSelected = i;
			break;
		}
	}
	// scroll start list
	__Menu_ScrollStartList();
	return ret;
}

void showAllGames(void)
{
	int i;
	u8 *id = NULL;
	if (gameSelected >= 0 && gameSelected < gameCnt) {
		id = gameList[gameSelected].id;
	}
	gameList = all_gameList;
	gameCnt = all_gameCnt;
	// find game selected
	gameStart = 0;
	gameSelected = 0;
	if (id) for (i=0; i<gameCnt; i++) {
		if (strncmp((char*)gameList[i].id, (char*)id, 6) == 0) {
			gameSelected = i;
			break;
		}
	}
	// scroll start list
	__Menu_ScrollStartList();
}

int filter_games_set(int type, int index)
{
	int ret = -1;
	switch (type) {
		case FILTER_ALL:
			showAllGames();
			ret = 0;
			index = -1;
			break;
		case FILTER_ONLINE:
			ret = filter_games(filter_online, "", 0);
			index = -1;
			break;
		case FILTER_UNPLAYED:
			ret = filter_games(filter_play_count, "", 0);
			index = -1;
			break;
		case FILTER_GENRE:
			ret = filter_games(filter_genre, genreTypes[index][0], 0);
			break;
		case FILTER_FEATURES:
			ret = filter_games(filter_features, featureTypes[index][0], 0);
			break;
		case FILTER_CONTROLLER:
			ret = filter_games(filter_controller, accessoryTypes[index][0], 0);
			break;
		case FILTER_GAMECUBE:
			ret = filter_games(filter_gamecube, "", 0);
			break;
		case FILTER_WII:
			ret = filter_games(filter_wii, "", 0);
			break;
		case FILTER_CHANNEL:
			ret = filter_games(filter_channel, "", 0);
			break;
	}
	if (ret > -1) {
		filter_type = type;
		filter_index = index;
	}
	return ret;
}

bool is_filter(int type, int index)
{
	if (type != filter_type) return false;
	switch (type) {
		case FILTER_GENRE:
		case FILTER_CONTROLLER:
		case FILTER_FEATURES:
			return index == filter_index;
	}
	return true;
}

char* mark_filter(int type, int index)
{
	return is_filter(type, index) ? "*" : " ";
}

s32 __sort_play_date(struct discHdr * hdr1, struct discHdr * hdr2, bool desc)
{
	int ret;
	if (desc)
		ret = (getLastPlay(hdr2->id) - getLastPlay(hdr1->id));
	else
		ret = (getLastPlay(hdr1->id) - getLastPlay(hdr2->id));
	if (ret == 0) ret = __sort_title(hdr1, hdr2, desc);
	return ret;
}

s32 __sort_install_date(struct discHdr * hdr1, struct discHdr * hdr2, bool desc)
{
	int ret = 0;
	void *p1, *p2;
	time_t t1 = 0;
	time_t t2 = 0;
	p1 = hmap_get(&install_time, hdr1->id);
	p2 = hmap_get(&install_time, hdr2->id);
	if (p1) memcpy(&t1, p1, sizeof(time_t));
	if (p2) memcpy(&t2, p2, sizeof(time_t));
	ret = t1 - t2;
	if (desc) ret = -ret;
	if (ret == 0) ret = __sort_title(hdr1, hdr2, desc);
	return ret;
}

s32 __sort_title(struct discHdr * hdr1, struct discHdr * hdr2, bool desc)
{
	char *title1 = get_title(hdr1);
	char *title2 = get_title(hdr2);
	title1 = skip_sort_ignore(title1);
	title2 = skip_sort_ignore(title2);
	if (desc)
		return mbs_coll(title2, title1);
	else
		return mbs_coll(title1, title2);
}

s32 __sort_releasedate(struct discHdr * hdr1, struct discHdr * hdr2, bool desc)
{
	int ret = 0;
	gameXMLinfo *g1 = get_game_info_id(hdr1->id);
	gameXMLinfo *g2 = get_game_info_id(hdr2->id);
	if (g1 && g2) {
		int date1 = (g1->year * 10000) + (g1->month * 100) + (g1->day);
		int date2 = (g2->year * 10000) + (g2->month * 100) + (g2->day);
		ret = (date1 - date2);
		if (desc) ret = -ret;
	}
	if (ret == 0) ret = __sort_title(hdr1, hdr2, desc);
	return ret;
}

s32 __sort_players(struct discHdr * hdr1, struct discHdr * hdr2, bool desc)
{
	int ret = 0;
	gameXMLinfo *g1 = get_game_info_id(hdr1->id);
	gameXMLinfo *g2 = get_game_info_id(hdr2->id);
	if (g1 && g2) {
		ret = (g1->players - g2->players);
		if (desc) ret = -ret;
	}
	if (ret == 0) ret = __sort_title(hdr1, hdr2, desc);
	return ret;
}

s32 __sort_wifiplayers(struct discHdr * hdr1, struct discHdr * hdr2, bool desc)
{
	int ret = 0;
	gameXMLinfo *g1 = get_game_info_id(hdr1->id);
	gameXMLinfo *g2 = get_game_info_id(hdr2->id);
	if (g1 && g2) {
		ret = (g1->wifiplayers - g2->wifiplayers);
		if (desc) ret = -ret;
	}
	if (ret == 0) ret = __sort_title(hdr1, hdr2, desc);
	return ret;
}

s32 __sort_pub(struct discHdr * hdr1, struct discHdr * hdr2, bool desc)
{
	int ret = 0;
	gameXMLinfo *g1 = get_game_info_id(hdr1->id);
	gameXMLinfo *g2 = get_game_info_id(hdr2->id);
	if (g1 && g2) {
		ret = mbs_coll(g1->publisher, g2->publisher);
		if (desc) ret = -ret;
	}
	if (ret == 0) ret = __sort_title(hdr1, hdr2, desc);
	return ret;
}

s32 __sort_dev(struct discHdr * hdr1, struct discHdr * hdr2, bool desc)
{
	int ret = 0;
	gameXMLinfo *g1 = get_game_info_id(hdr1->id);
	gameXMLinfo *g2 = get_game_info_id(hdr2->id);
	if (g1 && g2) {
		ret = mbs_coll(g1->developer, g2->developer);
		if (desc) ret = -ret;
	}
	if (ret == 0) ret = __sort_title(hdr1, hdr2, desc);
	return ret;
}

s32 __sort_play_count(struct discHdr * hdr1, struct discHdr * hdr2, bool desc)
{
	int ret;
	if (desc)
		ret = (getPlayCount(hdr2->id) - getPlayCount(hdr1->id));
	else
		ret = (getPlayCount(hdr1->id) - getPlayCount(hdr2->id));
	if (ret == 0) ret = __sort_title(hdr1, hdr2, desc);
	return ret;
}

s32 __sort_players_asc(const void *a, const void *b)
{
	return __sort_players((struct discHdr *)a, (struct discHdr *)b, 0);
}

s32 __sort_players_desc(const void *a, const void *b)
{
	return __sort_players((struct discHdr *)a, (struct discHdr *)b, 1);
}

s32 __sort_wifiplayers_asc(const void *a, const void *b)
{
	return __sort_wifiplayers((struct discHdr *)a, (struct discHdr *)b, 0);
}

s32 __sort_wifiplayers_desc(const void *a, const void *b)
{
	return __sort_wifiplayers((struct discHdr *)a, (struct discHdr *)b, 1);
}

s32 __sort_pub_asc(const void *a, const void *b)
{
	return __sort_pub((struct discHdr *)a, (struct discHdr *)b, 0);
}

s32 __sort_pub_desc(const void *a, const void *b)
{
	return __sort_pub((struct discHdr *)a, (struct discHdr *)b, 1);
}

s32 __sort_dev_asc(const void *a, const void *b)
{
	return __sort_dev((struct discHdr *)a, (struct discHdr *)b, 0);
}

s32 __sort_dev_desc(const void *a, const void *b)
{
	return __sort_dev((struct discHdr *)a, (struct discHdr *)b, 1);
}

s32 __sort_releasedate_asc(const void *a, const void *b)
{
	return __sort_releasedate((struct discHdr *)a, (struct discHdr *)b, 0);
}

s32 __sort_releasedate_desc(const void *a, const void *b)
{
	return __sort_releasedate((struct discHdr *)a, (struct discHdr *)b, 1);
}

s32 __sort_play_count_asc(const void *a, const void *b)
{
	return __sort_play_count((struct discHdr *)a, (struct discHdr *)b, 0);
}

s32 __sort_play_count_desc(const void *a, const void *b)
{
	return __sort_play_count((struct discHdr *)a, (struct discHdr *)b, 1);
}

s32 __sort_title_asc(const void *a, const void *b)
{
	return __sort_title((struct discHdr *)a, (struct discHdr *)b, 0);
}

s32 __sort_title_desc(const void *a, const void *b)
{
	return __sort_title((struct discHdr *)a, (struct discHdr *)b, 1);
}

s32 __sort_install_date_asc(const void *a, const void *b)
{
	return __sort_install_date((struct discHdr *)a, (struct discHdr *)b, 0);
}

s32 __sort_install_date_desc(const void *a, const void *b)
{
	return __sort_install_date((struct discHdr *)a, (struct discHdr *)b, 1);
}

s32 __sort_play_date_asc(const void *a, const void *b)
{
	return __sort_play_date((struct discHdr *)a, (struct discHdr *)b, 0);
}

s32 __sort_play_date_desc(const void *a, const void *b)
{
	return __sort_play_date((struct discHdr *)a, (struct discHdr *)b, 1);
}

void sortList(int (*sortFunc) (const void *, const void *))
{
	int inst_time = 0;
	if (sortFunc == __sort_install_date_asc	|| sortFunc == __sort_install_date_desc) {
		inst_time = 1;
		// prepare install times
		char fname[1024];
		struct stat st;
		int i;
		int ret;
		time_t time;
		//dbg_time1();
		printf("\n\n");
		hmap_init(&install_time, 6, sizeof(time_t));
		for (i=0; i<gameCnt; i++) {
			printf_("... %3d%%\r", 100*(i+1)/gameCnt);
			__console_flush(1);
			*fname = 0;
			time = 0;
			ret = WBFS_FAT_find_fname(gameList[i].id, fname, sizeof(fname));
			if (ret > 0 && stat(fname, &st) == 0) {
				time = st.st_mtime;
			}
			hmap_add(&install_time, gameList[i].id, &time);
		}
		//dbg_time2("stat"); Menu_PrintWait();
	}
	qsort(gameList, gameCnt, sizeof(struct discHdr), sortFunc);
	if (inst_time) {
		hmap_close(&install_time);
	}
	// scroll start list
	__Menu_ScrollStartList();
}

void sortList_default()
{
	sortList(default_sort_function);
}

void sortList_set(int index, bool desc)
{
	if (index < 0 || index > sortCnt) index = 0;
	sort_index = index;
	sort_desc = desc;
	if (sort_desc) {
		sortList(sortTypes[sort_index].sortDsc);
	} else {
		sortList(sortTypes[sort_index].sortAsc);
	}
}

int Menu_Filter()			//Filter by Genre
{
	struct discHdr *header = NULL;
	int redraw_cover = 0;
	struct Menu menu;
	int rows, cols, size;
	CON_GetMetrics(&cols, &rows);
	if ((size = rows-12) < 3) size = 3;		//scroll area -7 fixed and 5 overhead
	menu_init(&menu, genreCnt+7);			//Add 7 fixed menu items
	for (;;) {

		menu.line_count = 0;

		if (gameCnt) {
			header = &gameList[gameSelected];
		} else {
			header = NULL;
		}
		int n;
		Con_Clear();
		FgColor(CFG.color_header);
		printf_x(gt("Filter by Genre"));
		printf(":\n\n");
		MENU_MARK();
		printf("<%s>\n", gt("Filter by Controller"));
		MENU_MARK();
		printf("<%s>\n", gt("Filter by Online Features"));
		MENU_MARK();
		printf("%s %s\n", mark_filter(FILTER_ALL,-1), gt("All Games"));
		MENU_MARK();
		printf("%s %s\n", mark_filter(FILTER_UNPLAYED,-1), gt("Unplayed Games"));
		MENU_MARK();
		printf("%s %s\n", mark_filter(FILTER_WII,-1), gt("Wii"));
		MENU_MARK();
		printf("%s %s\n", mark_filter(FILTER_GAMECUBE,-1), gt("GameCube"));
		MENU_MARK();
		printf("%s %s\n", mark_filter(FILTER_CHANNEL,-1), gt("Channel"));
		menu_window_begin(&menu, size, genreCnt);
		for (n=0;n<genreCnt;n++) {
			if (menu_window_mark(&menu))
				printf("%s %s\n", mark_filter(FILTER_GENRE,n), genreTypes[n][1]);
		}
		DefaultColor();
		menu_window_end(&menu, cols);
		
		//printf("\n");
		printf_h(gt("Press %s to select filter type"), (button_names[CFG.button_confirm.num]));
		__console_flush(0);

		if (redraw_cover) {
			if (header) Gui_DrawCover(header->id);
			redraw_cover = 0;
		}
		
		u32 buttons = Wpad_WaitButtonsRpt();
		menu_move(&menu, buttons);

		int change = 0;
		if (buttons & WPAD_BUTTON_LEFT) change = -1;
		if (buttons & WPAD_BUTTON_RIGHT || buttons & CFG.button_confirm.mask || buttons & CFG.button_save.mask) change = +1;

		if (change) {
			if (0 == menu.current) {
				Menu_Filter2();
				redraw_cover = 1;
				break;
			} else if (1 == menu.current) {
				Menu_Filter3();
				redraw_cover = 1;
				break;
			} else if (2 == menu.current) {
				filter_games_set(FILTER_ALL, -1);
				redraw_cover = 1;
			} else if (3 == menu.current) {
				redraw_cover = 1;
				filter_games_set(FILTER_UNPLAYED, -1);
			}
			else if (4 == menu.current) {
				redraw_cover = 1;
				filter_games_set(FILTER_WII, -1);
			}
			else if (5 == menu.current) {
				redraw_cover = 1;
				filter_games_set(FILTER_GAMECUBE, -1);
			}
			else if (6 == menu.current) {
				redraw_cover = 1;
				filter_games_set(FILTER_CHANNEL, -1);
			}
			n = menu.current - 7;
			if (n >= 0 && n < genreCnt) {
				redraw_cover = 1;
				filter_games_set(FILTER_GENRE, n);
			}
		}
		
		// HOME button
		if (buttons & CFG.button_exit.mask) {
			Handle_Home(0);
		}
		if (buttons & CFG.button_cancel.mask) break;
	}
	return 0;
}

int Menu_Filter2()				//Filter by Controler
{
	struct discHdr *header = NULL;
	int redraw_cover = 0;
	struct Menu menu;
	int rows, cols, size;
	CON_GetMetrics(&cols, &rows);
	if ((size = rows-11) < 3) size = 3;		//scroll area -6 fixed and 5 overhead
	menu_init(&menu, accessoryCnt+6);		//Add 6 fixed menu items
	for (;;) {

		menu.line_count = 0;

		if (gameCnt) {
			header = &gameList[gameSelected];
		} else {
			header = NULL;
		}
		int n;
		Con_Clear();
		FgColor(CFG.color_header);
		printf_x(gt("Filter by Controller"));
		printf(":\n\n");
		MENU_MARK();
		printf("<%s>\n", gt("Filter by Genre"));
		MENU_MARK();
		printf("<%s>\n", gt("Filter by Online Features"));
		MENU_MARK();
		printf("%s %s\n", mark_filter(FILTER_ALL,-1), gt("All Games"));
		MENU_MARK();
		printf("%s %s\n", mark_filter(FILTER_WII,-1), gt("Wii"));
		MENU_MARK();
		printf("%s %s\n", mark_filter(FILTER_GAMECUBE,-1), gt("GameCube"));
		MENU_MARK();
		printf("%s %s\n", mark_filter(FILTER_CHANNEL,-1), gt("Channel"));
		menu_window_begin(&menu, size, accessoryCnt);
		for (n=0; n<accessoryCnt; n++) {
			if (menu_window_mark(&menu)) {
				printf("%s ", mark_filter(FILTER_CONTROLLER,n));
				printf("%s\n", gt(accessoryTypes[n][1]));
			}
		}
		DefaultColor();
		menu_window_end(&menu, cols);
	
		printf_h(gt("Press %s to select filter type"), (button_names[CFG.button_confirm.num]));
		__console_flush(0);

		if (redraw_cover) {
			if (header) Gui_DrawCover(header->id);
			redraw_cover = 0;
		}
		
		u32 buttons = Wpad_WaitButtonsRpt();
		menu_move(&menu, buttons);

		int change = 0;
		if (buttons & WPAD_BUTTON_LEFT) change = -1;
		if (buttons & WPAD_BUTTON_RIGHT || buttons & CFG.button_confirm.mask || buttons & CFG.button_save.mask) change = +1;

		if (change) {
			if (0 == menu.current) {
				Menu_Filter();
				redraw_cover = 1;
				goto end;
			} else if (1 == menu.current) {
				Menu_Filter3();
				redraw_cover = 1;
				goto end;
			} else if (2 == menu.current) {
				filter_games_set(FILTER_ALL, -1);
				redraw_cover = 1;
			}
			else if (3 == menu.current) {
				redraw_cover = 1;
				filter_games_set(FILTER_WII, -1);
			}
			else if (4 == menu.current) {
				filter_games_set(FILTER_GAMECUBE, -1);
				redraw_cover = 1;
			}
			else if (5 == menu.current) {
				redraw_cover = 1;
				filter_games_set(FILTER_CHANNEL, -1);
			}
			n = menu.current - 6;
			if (n >= 0 && n < accessoryCnt) {
				redraw_cover = 1;
				filter_games_set(FILTER_CONTROLLER, n);
			}
		}
		
		// HOME button
		if (buttons & CFG.button_exit.mask) {
			Handle_Home(0);
		}
		if (buttons & CFG.button_cancel.mask) break;
	}
	end:
	return 0;
}

int Menu_Filter3()			//Filter by Online Features
{
	struct discHdr *header = NULL;
	int redraw_cover = 0;
	struct Menu menu;
	int rows, cols, size;
	CON_GetMetrics(&cols, &rows);
	if ((size = rows-12) < 3) size = 3;		//scroll area -7 fixed and 5 overhead
	menu_init(&menu, featureCnt+7);			//Add 7 fixed menu items
	for (;;) {

		menu.line_count = 0;

		if (gameCnt) {
			header = &gameList[gameSelected];
		} else {
			header = NULL;
		}
		int n;
		Con_Clear();
		FgColor(CFG.color_header);
		printf_x(gt("Filter by Online Features"));
		printf(":\n\n");
		MENU_MARK();
		printf("<%s>\n", gt("Filter by Genre"));
		MENU_MARK();
		printf("<%s>\n", gt("Filter by Controller"));		
		MENU_MARK();
		printf("%s %s\n", mark_filter(FILTER_ALL,-1), gt("All Games"));
		MENU_MARK();
		printf("%s %s\n", mark_filter(FILTER_WII,-1), gt("Wii"));
		MENU_MARK();
		printf("%s %s\n", mark_filter(FILTER_GAMECUBE,-1), gt("GameCube"));
		MENU_MARK();
		printf("%s %s\n", mark_filter(FILTER_CHANNEL,-1), gt("Channel"));
		MENU_MARK();
		printf("%s %s\n", mark_filter(FILTER_ONLINE,-1), gt("Online Play"));
		menu_window_begin(&menu, size, featureCnt);
		for (n=0;n<featureCnt;n++) {
			if (menu_window_mark(&menu))
			printf("%s ", mark_filter(FILTER_FEATURES,n));
			printf("%s\n", gt(featureTypes[n][1]));
		}
		DefaultColor();
		menu_window_end(&menu, cols);
		
		printf_h(gt("Press %s to select filter type"), (button_names[CFG.button_confirm.num]));
		__console_flush(0);

		if (redraw_cover) {
			if (header) Gui_DrawCover(header->id);
			redraw_cover = 0;
		}
		
		u32 buttons = Wpad_WaitButtonsRpt();
		menu_move(&menu, buttons);

		int change = 0;
		if (buttons & WPAD_BUTTON_LEFT) change = -1;
		if (buttons & WPAD_BUTTON_RIGHT || buttons & CFG.button_confirm.mask || buttons & CFG.button_save.mask) change = +1;

		if (change) {
			if (0 == menu.current) {
				Menu_Filter();
				redraw_cover = 1;
				goto end;
			} else if (1 == menu.current) {
				Menu_Filter2();
				redraw_cover = 1;
				goto end;
			} else if (2 == menu.current) {
				filter_games_set(FILTER_ALL, -1);
				redraw_cover = 1;
			}
			else if (3 == menu.current) {
				redraw_cover = 1;
				filter_games_set(FILTER_WII, -1);
			}
			else if (4 == menu.current) {
				filter_games_set(FILTER_GAMECUBE, -1);
				redraw_cover = 1;
			}
			else if (5 == menu.current) {
				redraw_cover = 1;
				filter_games_set(FILTER_CHANNEL, -1);
			}
			else if (6 == menu.current) {
				redraw_cover = 1;
				filter_games_set(FILTER_ONLINE, -1);
			}
			n = menu.current - 7;
			if (n >= 0 && n < featureCnt) {
				redraw_cover = 1;
				filter_games_set(FILTER_FEATURES, n);
			}
		}
		
		// HOME button
		if (buttons & CFG.button_exit.mask) {
			Handle_Home(0);
		}
		if (buttons & CFG.button_cancel.mask) break;
	}
	end:
	return 0;
}

int Menu_Sort()
{
	struct discHdr *header = NULL;
	int redraw_cover = 0;
	int n = 0;
	bool first_run = 1;
	bool descend[sortCnt];
	for (;n<sortCnt;n++)
		descend[n] = 0;
	struct Menu menu;
	menu_init(&menu, sortCnt);
	for (;;) {

		menu.line_count = 0;

		if (gameCnt) {
			header = &gameList[gameSelected];
		} else {
			header = NULL;
		}
		Con_Clear();
		FgColor(CFG.color_header);
		printf_x(gt("Choose a sorting method"));
		printf(":\n\n");
		for (n=0; n<sortCnt; n++) {
			if (sort_index == n && sort_desc && first_run) {
				first_run = 0;
				descend[n] = 1;
			}
			MENU_MARK();
			printf("%s ", (sort_index == n ? "*": " "));
			printf("%s", con_align(sortTypes[n].name,25));
			printf("%s\n", !descend[n] ? gt("< ASC  >") : gt("< DESC >"));
		}
		DefaultColor();

		printf("\n");
		printf_h(gt("Press %s to select sorting method"), (button_names[CFG.button_confirm.num]));
		__console_flush(0);

		if (redraw_cover) {
			if (header) Gui_DrawCover(header->id);
			redraw_cover = 0;
		}
		
		u32 buttons = Wpad_WaitButtonsRpt();
		menu_move(&menu, buttons);

		int change = 0;
		if (buttons & WPAD_BUTTON_LEFT || buttons & WPAD_BUTTON_RIGHT) change = -1;
		if (buttons & CFG.button_confirm.mask || buttons & CFG.button_save.mask) change = +1;

		if (change) {
			n = menu.current;
			if (n >= 0 && n < sortCnt) {
				if (change == -1) descend[n] = !descend[n];
				else {
					if (descend[n]) {
						sort_desc = 1;
						sortList(sortTypes[n].sortDsc);
					} else {
						sort_desc = 0;
						sortList(sortTypes[n].sortAsc);
					}
					redraw_cover = 1;
					sort_index = n;
				}
			}
		}
		
		// HOME button
		if (buttons & CFG.button_exit.mask) {
			Handle_Home(0);
		}
		if (buttons & CFG.button_cancel.mask) break;
	}
	return 0;
}