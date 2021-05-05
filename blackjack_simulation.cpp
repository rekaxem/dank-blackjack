#include <bits/stdc++.h>
using namespace std;

const double multiplier=1.6;

int stand_winrate[21][14][2], stand_tierate[21][14][2]; // sum of cards, dealer's upcard, numerator/denominator
double game_winrate[6][22][2][14], game_tierate[6][22][2][14]; // number of cards, sum of cards, with/without ace, dealer's upcard
int optimal[6][22][2][14]; // 0 = impossible, -1 = won, 1 = hit, 2 = stand

void calcstand() {
	for (int player=17; player<=20; player++) {
		int dealer[5];
		for (dealer[0]=1; dealer[0]<=13; dealer[0]++) {
			int wincount=0, tiecount=0, total=0;
			for (dealer[1]=1; dealer[1]<=13; dealer[1]++) {
				if (dealer[0]==1 && dealer[1]>=10 || dealer[0]>=10 && dealer[1]==1) continue;
				for (dealer[2]=1; dealer[2]<=13; dealer[2]++) {
					for (dealer[3]=1; dealer[3]<=13; dealer[3]++) {
						for (dealer[4]=1; dealer[4]<=13; dealer[4]++) {
							total++;
							int sum=0, ace=0;
							for (int i=0; i<5; i++) {
								sum+=min(10, dealer[i]);
								if (dealer[i]==1) ace=1;
								if (i==4 && sum<=21) break;
								if (sum>21) {
									wincount++;
									break;
								}
								if (sum>=17) {
									if (player>sum) wincount++;
									else if (player==sum) tiecount++;
									break;
								}
								if (ace==1 && sum>=7 && sum<=11) {
									if (player>sum+10) wincount++;
									else if (player==sum+10) tiecount++;
									break;
								}
							}
						}
					}
				}
			}
			stand_winrate[player][dealer[0]][0]=wincount, stand_tierate[player][dealer[0]][0]=tiecount;
			stand_winrate[player][dealer[0]][1]=stand_tierate[player][dealer[0]][1]=total;
		}
	}
	for (int player=4; player<=16; player++) {
		for (int dealer=1; dealer<=13; dealer++) {
			stand_winrate[player][dealer][0]=stand_winrate[17][dealer][0];
			stand_winrate[player][dealer][1]=stand_tierate[player][dealer][1]=stand_winrate[17][dealer][1];
		}
	}
}
pair<double, double> calcrate(int card, int sum, int ace, int dealer) { // return {winrate, tierate}
	if (optimal[card][sum][ace][dealer]!=0) {
		return make_pair(game_winrate[card][sum][ace][dealer], game_tierate[card][sum][ace][dealer]);
	}
	if (card==5 || sum==21 || ace==1 && sum==11) {
		game_winrate[card][sum][ace][dealer]=1.0, game_tierate[card][sum][ace][dealer]=0.0;
		optimal[card][sum][ace][dealer]=-1;
		return make_pair(1.0, 0.0);
	}
	pair<double, double> res=calcrate(card+1, sum+1, 1, dealer);
	for (int player=2; player<=13; player++) {
		if (sum+min(10, player)>21) continue;
		pair<double, double> rates=calcrate(card+1, sum+min(10, player), ace, dealer);
		res.first+=rates.first, res.second+=rates.second;
	}
	res.first/=13.0, res.second/=13.0;
	int standsum=sum;
	if (ace==1 && sum<11) standsum+=10;
	if ((1.0+multiplier)*stand_winrate[standsum][dealer][0]/stand_winrate[standsum][dealer][1]+1.0*stand_tierate[standsum][dealer][0]/stand_tierate[standsum][dealer][1]<(1.0+multiplier)*res.first+1.0*res.second) {
		optimal[card][sum][ace][dealer]=1;
	} else {
		optimal[card][sum][ace][dealer]=2;
		res=make_pair(1.0*stand_winrate[standsum][dealer][0]/stand_winrate[standsum][dealer][1], 1.0*stand_tierate[standsum][dealer][0]/stand_tierate[standsum][dealer][1]);
	}
	game_winrate[card][sum][ace][dealer]=res.first, game_tierate[card][sum][ace][dealer]=res.second;
	return res;
}
void calculate() {
	calcstand();
	for (int dealer=1; dealer<=13; dealer++) {
		for (int player=4; player<=20; player++) calcrate(2, player, 0, dealer);
		for (int player=2; player<=10; player++) calcrate(2, player, 1, dealer);
	}
}
int main() {
	calculate();
	srand(0);
	const int test_cnt=10000000;
	const long long int initial=10000000;
	int wincnt=0, tiecnt=0, losecnt=0;
	long long int balance=initial, bet=250000;
	for (int test=0; test<test_cnt; test++) {
		if (balance<bet) {
			cout<<"Lost all at test "<<test+1<<'\n';
			break;
		}
		int p[5], d[5];
		for (int i=0; i<5; i++) p[i]=rand()%13+1, d[i]=rand()%13+1;
		while (p[0]==1 && p[1]>=10 || p[0]>=10 && p[1]==1) p[0]=rand()%13+1, p[1]=rand()%13+1;
		while (d[0]==1 && d[1]>=10 || d[0]>=10 && d[1]==1) d[0]=rand()%13+1, d[1]=rand()%13+1;
		int dsum=0, dace=0, dbusted=0, dfivecards=0;
		for (int i=0; i<5; i++) {
			dsum+=min(10, d[i]);
			if (d[i]==1) dace=1;
			if (dsum>21) {
				dbusted=1;
				break;
			}
			if (i==4) {
				dfivecards=1;
				break;
			}
			if (dsum>=17 || dace==1 && dsum>=7 && dsum<=11) {
				break;
			}
		}
		int psum=min(10, p[0])+min(10, p[1]), pace=(p[0]==1 || p[1]==1);
		for (int i=2; i<5; i++) {
			int res=optimal[i][psum][pace][d[0]];
			if (res==-1) {
				cout<<"already won";
				return 0;
			} else if (res==0) {
				cout<<"impossible";
				return 0;
			} else if (res==1) {
				psum+=min(10, p[i]);
				if (p[i]==1) pace=1;
			} else if (res==2) {
				if (pace==1 && psum<=11) psum+=10;
				if (dace==1 && dsum<=11) dsum+=10;
				if (dbusted==1) {
					wincnt++;
					balance+=1.0*bet*multiplier;
				} else if (dsum==21 || dfivecards==1) {
					losecnt++;
					balance-=bet;
				} else if (dsum==psum) {
					tiecnt++;
				} else if (dsum>psum) {
					losecnt++;
					balance-=bet;
				} else if (dsum<psum) {
					wincnt++;
					balance+=1.0*bet*multiplier;
				} else {
					cout<<"warning";
					return 0;
				}
				break;
			}
			if (psum>21) {
				losecnt++;
				balance-=bet;
				break;
			}
			if (psum==21 || pace==1 && psum==11 || i==4) {
				wincnt++;
				balance+=1.0*bet*multiplier;
				break;
			}
		}
	}
	cout<<"RESULTS - current multiplier: "<<multiplier<<"\nWin: "<<wincnt<<'/'<<test_cnt<<", Tie: "<<tiecnt<<'/'<<test_cnt<<", Lose: "<<losecnt<<'/'<<test_cnt<<"\nBalance: "<<balance<<", average earnings per game: "<<1.0*(balance-initial)/test_cnt<<" ("<<100.0*(balance-initial)/test_cnt/bet<<"%)\n";
}
