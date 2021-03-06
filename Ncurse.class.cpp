/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ncurse.class.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: evanheum <evanheum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/13 00:06:31 by evanheum          #+#    #+#             */
/*   Updated: 2018/01/14 23:14:40 by evanheum         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ncurse.hpp"
#include "Player.hpp"
#include "Enemey.hpp"
#include "Tracker.hpp"
#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include <iomanip>
#include <string>
#include <ctime>

Ncurse::Ncurse() : _time(clock()){
	initscr();
	noecho();
	cbreak();
	timeout(0);
	getmaxyx(stdscr, _row, _col);
	WINDOW *gamewin = newwin(_row, _col, 0, 0);
	_enemies = 20;
	refresh();
	mvwprintw(gamewin ,_row/2, (_col-std::strlen("SPACE INVADERS"))/2, "%s", "SPACEINVADERS");
	wrefresh(gamewin);
	curs_set(0);
	getch();
	if (setMenu()) {
		delwin(gamewin);
		setGameEnv();
	}
	delwin(gamewin);
}

Ncurse::Ncurse(Ncurse const &src) {
	*this = src;
	this->operator=(src);
}

Ncurse::~Ncurse() {
	endwin();
}

bool	Ncurse::setMenu() {
	std::string choices[2] = {"new game","quit game"};
	int choice;
	int highlight = 0;
	bool boom;
	WINDOW *menu = newwin(6, this->_col/4, this->_row/1.9, _col/2.7);
	wborder(menu, '-', '-', '-','-','-','-','-','-');
	keypad(menu, true);
	wrefresh(menu);
	while (1) {
		for (int i = 0; i < 2; i++) {
			if ( i == highlight)
				wattron(menu, A_STANDOUT);
			mvwprintw(menu, i+1, (this->_col/4)/2, choices[i].c_str());
			 wattroff(menu, A_STANDOUT);
			wrefresh(menu);
		}
		choice = wgetch(menu);
		switch(choice) {
			case KEY_UP:
				highlight--;
				if (highlight == -1)
					highlight = 1;
				break;
			case KEY_DOWN:
				highlight++;
				if (highlight == 2)
					highlight = 0;
				break;
			default:
				break;
		}
		std::string c = choices[highlight].c_str();
		if (choice == 10) {
			if(!c.compare("new game")) {
				boom = true;
				break;
			}
			else if (!c.compare("quit game")) {
				boom = false;
				break;
			}
		}
	}
	clear();
	delwin(menu);
	return boom;
}

void	Ncurse::setGameEnv() {
	getmaxyx(stdscr, this->_row, this->_col);
	WINDOW 		*gamewin = newwin(this->_row - 8, this->_col - 3, 5, 1);
	WINDOW 		*score = newwin(5, this->_col - 3, 0, 1);
	WINDOW		*control = newwin(3, this->_col - 3, this->_row - 3, 1);
	//int frames = 0;
	refresh();
	box(gamewin, 0, 0);
	box(score, 0, 0);
	box(control, 0, 0);
	mvwprintw(control, 1, 20,
	"CONTROLS:      [SPACEBAR]: Shoot      |      [KEY_ARROWS]: Movement     |     [Q]: Exit     |");
	timeout(0);
	wrefresh(gamewin);
	wrefresh(control);
	wrefresh(score);
	int esc = 0;
	int scoretrack = 0;
	keypad(gamewin, true);
	nodelay(gamewin, true);
	wrefresh(gamewin);
	// Create entity tracker pls.
	Tracker tracker;

	Player *player = new Player(this->_row - 8 - 1, this->_col - 3 - 1, ((this->_col - 3 - 1) / 2), (this->_row - 8 - 1) - 2);
	tracker.addEntity(player);

	//Enemey *enemey = new Enemey(this->_row - 8 - 1, this->_col - 3 - 1, 2, 2);
	//tracker.addEntity(enemey);
	//
	for (int i = 0; i < _enemies; i++) {
		tracker.createEnemey(this->_row - 8 - 1, this->_col - 3 - 1, 2 + rand() % ((this->_col - 3 - 1)), 2 + rand() % 10);
	}

	for (int i = 0; i < 20; i ++) {
		tracker.createBg(this->_row - 8 - 1, this->_col - 3 - 1, 2 + rand() % ((this->_col - 3 - 1)), 2 + rand() % ((this->_row - 8 - 1)));
	}

	mvwprintw(score, 2, 30, "LIVES: %d", player->getLives());
	//mvwprintw(score, 2, 50, "ENEMIES: %d", tracker.getNumOfEnemeies());
	wrefresh(score);

	getch();
	int		check = player->getLives();
	int 	check2 = tracker.getNumOfEnemeies() + 1;
	while (1) {
		if (player->getLives() < 1) {
			break;
		}
		//start = clock();
		//frames++;
		esc = wgetch(gamewin);
		switch (esc) {
			case KEY_LEFT:
				player->moveProjectile(tracker, gamewin, MOVE_LEFT);
				break;
			case KEY_RIGHT:
				player->moveProjectile(tracker, gamewin, MOVE_RIGHT);
				break;
			case ' ':
				if (player->readyToFire()) {
					player->fire();
					tracker.createProjectile(this->_row - 8 - 1, this->_col - 3 - 1, player->getpY(), player->getpX() - 1, true);
				}
				break;
		}
		if (esc == 'q') {
			break;
		}
		if (player->getLives() != check || tracker.getNumOfEnemeies() != check2) {
			mvwprintw(score, 2, 30, "LIVES: %d", player->getLives());
			mvwprintw(score, 2, 50, "ENEMIES: %d", tracker.getNumOfEnemeies());
			mvwprintw(score, 2, 70, "SCORE: %d", scoretrack);
			bool wombo = false;
			if (tracker.getNumOfEnemeies() < 10)
				mvwaddch(score, 2, 60, ' ');
			if (check2 > tracker.getNumOfEnemeies())
			{
				if ((check2 - tracker.getNumOfEnemeies()) > 1) {
					wombo = true;
					scoretrack += ((check2 - tracker.getNumOfEnemeies()) - 1) * 5;
				}
				scoretrack += (check2 - tracker.getNumOfEnemeies()) * 10;
			}
			mvwprintw(score, 2, 70, "SCORE: %d", scoretrack);
			if (wombo)
				mvwprintw(score, 2, 90, "WOMBO COMBO!");
			else
				mvwprintw(score, 2, 90, "            ");
			wrefresh(score);
			check = player->getLives();
			check2 = tracker.getNumOfEnemeies();
		}
		if (check2 == 0)
			break;
		// player.display();

		//mvwprintw(score, 2, 30, "TICKEDENT: %d", tracker.entitiesTicked);
		//wrefresh(score);
		tracker.tickEntities(tracker, gamewin);
		//wrefresh(gamewin);
		//if (clock()/ 100000 != _time / 100000) {
		//end = clock();
		//	mvwprintw(score, 2, 30, "FPS:%4d", (end - start));
		//	wrefresh(score);
		//	frames = 0;
		//}
		//while (clock() / CLOCKS_PER_FRAME == _time / CLOCKS_PER_FRAME) {}
	}
	delwin(gamewin);
	delwin(control);
	delwin(score);
	refresh();
	// clear();
	setGameOver(check2 == 0);
}


void	Ncurse::setGameOver(bool won) {
	WINDOW *gameover = newwin(this->_row, this->_col, 0, 0);
	refresh();
	getch();
	if (won) {
		mvwprintw(gameover, this->_row / 2, (_col-std::strlen("WINNER")) / 2, "%s", "WINNER");
	} else {
		mvwprintw(gameover ,this->_row / 2, (_col-std::strlen("LOSER")) / 2, "%s", "LOSER");
	}
	wrefresh(gameover);
	if(setMenu()) {
		if (won) {
			_enemies += 5;
		}
		delwin(gameover);
		setGameEnv();
	}
	delwin(gameover);
}

void	Ncurse::setcurseXY() {
	getmaxyx(stdscr, this->_row, this->_col);

}

void	Ncurse::setWindow(WINDOW *name) {
	this->_curwin = name;
}

void	Ncurse::setRow(int i) {
	this->_row = i;
}

void	Ncurse::setCol(int i) {
	this->_col = i;
}

void	Ncurse::clearWindow() {
		clear();
		refresh();
		getch();
}

int		Ncurse::getRow(void) const {
	return this->_row;
}

int		Ncurse::getCol(void) const {
	return this->_col;
}

WINDOW		*Ncurse::getWindow() {
	return this->_curwin;
}


Ncurse	&Ncurse::operator=(Ncurse const &rhs) {
	if (this != &rhs){
		this->_col = rhs._col;
		this->_row = rhs._row;
		*this = rhs;
	}
	return *this;
}
