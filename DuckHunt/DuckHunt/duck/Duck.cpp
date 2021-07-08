#include <duck/Duck.h>
#include <SDL.h>
#include <SDL_image.h>
#include <stdlib.h>
#include <iostream>
#include <string>
//#include<windows.h>

Duck::Duck(int w, int h, int x, int y) : _w(w), _h(h), _x(x), _y(y)
{
	img = {_x, _y, _w, _h};
	alive = true;
	free = false;
	flipped = false;
}

Duck::~Duck()
{
}

void Duck::move()
{
	if (free && _y < 1000)
	{
		if (_x > 640)
		{
			_x += 5;
			flipped = true;
		}
		else
		{
			_x -= 5;
			flipped = false;
		}
		_y -= 5;
		img = {_x, _y, _w, _h};
	}
	else if (alive)
	{
		if (_y >= 450)
		{
			_y -= 4;
		}
		int dir = rand() % 8 + 1;
		int distance = rand() % 3 + 5;

		switch (dir)
		{
		case 1:
			if (_y - distance > 0)
				_y -= distance;
			break;
		case 2:
			if (_x + distance < 1180 && _y - distance > 0)
			{
				_x += distance;
				_y -= distance;
				flipped = true;
			}
			break;
		case 3:
			if (_x + distance < 1180)
			{
				_x += distance;
				flipped = true;
			}
			break;
		case 4:
			if (_x + distance < 1180 && _y + distance < 450)
			{
				_x += distance;
				_y += distance;
				flipped = true;
			}
			break;
		case 5:
			if (_y + distance < 450)
				_y += distance;
			break;
		case 6:
			if (_x - distance > 0 && _y + distance < 450)
			{
				_x -= distance;
				_y += distance;
				flipped = false;
			}
			break;
		case 7:
			if (_x - distance > 0)
			{
				_x -= distance;
				flipped = false;
			}
			break;
		case 8:
			if (_x - distance > 0 && _y - distance > 0)
			{
				_x -= distance;
				_y -= distance;
				flipped = false;
			}
			break;
		}

		img = {_x, _y, _w, _h};
	}
	else
	{
		if (_y++ < 700)
		{
			_y += 4;
			img = {_x, _y, _w, _h};
		}
	}
}

int Duck::getX()
{
	return _x;
}

bool Duck::isFlipped()
{
	return flipped;
}

int Duck::getY()
{
	return _y;
}

bool Duck::isAlive()
{
	return alive;
}

bool Duck::die() 
{
	alive = false;
	return alive;
}

bool Duck::flyAway()
{
	free = true;
	return free;
}

bool Duck::flown()
{
	return free;
}

bool Duck::spawn()
{

	_x = rand() % 1200 + 80;
	_y = 600;
	alive = true;
	free = false;

	return alive;
}

SDL_Rect *Duck::getRect()
{
	return &img;
}