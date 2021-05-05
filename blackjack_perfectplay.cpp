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
	cout<<"** Blackjack **\n\n";
	while (true) {
		int card, sum, ace, dealer;
		cout<<"Enter number of cards you have:\n";
		cin>>card;
		cout<<"Enter sum of your cards: (A=1, JQK=10)\n";
		cin>>sum;
		cout<<"Enter whether if you have ace: (0=No, 1=Yes)\n";
		cin>>ace;
		cout<<"Enter dealer's card: (A=1, JQK=10)\n";
		cin>>dealer;
		if (card>5 || card<2 || sum>21 || sum<2 || ace<0 || ace>1 || dealer<1 || dealer>13 || optimal[card][sum][ace][dealer]==0) cout<<"\n* Invalid input, please try again.\n\n";
		else cout<<"------\nYou should: "<<(optimal[card][sum][ace][dealer]==1 ? "Hit" : optimal[card][sum][ace][dealer]==2 ? "Stand" : "Do nothing. You have already won.")<<"\nWin: "<<game_winrate[card][sum][ace][dealer]<<", Tie: "<<game_tierate[card][sum][ace][dealer]<<", Lose: "<<1.0-game_winrate[card][sum][ace][dealer]-game_tierate[card][sum][ace][dealer]<<"\n------\n\n";
	}
}
