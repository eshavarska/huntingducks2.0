#include <player/Player.h>
#include <bridge.h>
#include <duck/Duck.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>

Player::Player()
{
}

Player::~Player()
{
}

bool Player::Shoot(Duck &duck, SDL_Rect *scope)
{
	if (duck.isAlive() && !duck.flown())
		if (Collision(duck.getRect(), scope))
		{
			std::cout << "shot" << std::endl;
			return true;
		}
		else
		{
			return false;
		}
	else
		return false;
}

bool Player::eventHandler(SDL_Event &event, Duck &duck, Duck &duck1, int &shotCount, int &score, Mix_Chunk *ded)
{
	bool checker = false;
	if (event.type == SDL_MOUSEBUTTONDOWN && SDL_BUTTON(SDL_GetMouseState(&curX, &curY)) == SDL_BUTTON_LEFT)
	{
		this->gBox.x = curX;
		this->gBox.y = curY;
		if (Shoot(duck, this->getRect()))
		{
			Mix_PlayChannel(-1, ded, 0);
			checker = true;
			duck.die();
			shotCount++;
			score += 1000;
		}
		if (Shoot(duck1, this->getRect()))
		{
			Mix_PlayChannel(-1, ded, 0);
			checker = true;
			duck1.die();
			shotCount++;
			score += 1000;
		}
	}
	return checker;
}

int Player::getScore()
{
	return Score;
}

void Player::resetScore()
{
	Score = 0;
}

void Player::incScore(int inc)
{
	Score += inc;
}

int Player::getCurX()
{
	return curX;
}

int Player::getCurY()
{
	return curY;
}

bool Player::die()
{
	alive = 0;
	return alive;
}

bool Player::isAlive()
{
	return alive;
}

SDL_Rect *Player::getRect()
{
	return &gBox;
}
