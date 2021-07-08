#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "ui/Engine.h"
#include "exceptions/SDL_exception.h"
#include <duck/Duck.h>
#include <player/Player.h>
#include <string>
#include <map>
#include <chrono>
#include <thread>
#include <sstream>
#include <iomanip>
//#include <Windows.h>

const int WIDTH = 1280, HEIGHT = 720;
const int DUCK_WIDTH = 100, DUCK_HEIGHT = 100;
int duck_pos_x = 350, duck_pos_y = 350;

std::string duck_img_path = "images/birds/fenix.png";
std::string duck_die_img_path = "images/birds/phoenix_dead.png";
std::string menu = "images/menu/menu.png";
std::string background_img_path = "images/menu/background.png";
std::string foreground_img_path = "images/menu/foreground.png";

std::string zero_shells = "images/ammo/0_shells.png";
std::string one_shell = "images/ammo/1_shell.png";
std::string two_shells = "images/ammo/2_shells.png";
std::string three_shells = "images/ammo/3_shells.png";

std::string two_white_fenixes = "images/birds/two_white_birds.png";
std::string two_red_fenixes = "images/birds/two_red_birds.png";
std::string red_white_fenixes = "images/birds/red_white_bird.png";

std::string one_white_fenix = "images/birds/white_bird.png";
std::string one_red_fenix = "images/birds/red_bird.png";

Duck duck1(DUCK_WIDTH, DUCK_HEIGHT, duck_pos_x, duck_pos_y);
Duck duck2(DUCK_WIDTH, DUCK_HEIGHT, duck_pos_x - 100, duck_pos_y - 100);
Player player;

TTF_Font* Langar;

SDL_Texture* duckTexture;
SDL_Texture* deadDuckTexture;
SDL_Texture* menuTexture;
SDL_Texture* grassTexture;
SDL_Texture* backgroundTexture;
SDL_Texture* zero_shell_texture;
SDL_Texture* one_shell_texture;
SDL_Texture* two_shell_texture;
SDL_Texture* three_shell_texture;
SDL_Texture* two_red_fenixes_texture;
SDL_Texture* two_white_fenixes_texture;
SDL_Texture* red_white_fenixes_texture;
SDL_Texture* one_white_fenix_texture;
SDL_Texture* one_red_fenix_texture;
SDL_Rect* rect;
SDL_Rect bgRect = { 0, 0, 1280, 720 };
SDL_Rect grassRect = { 0, 497, 1280, 223 };

SDL_Rect duckFrame;

int ammoCount = 3;
int shotFenixes = 0;
int levelCount = 1;
bool nextLevel = false;
int score = 0000;

clock_t start;
bool timerRunning = false;

std::map<int, int> shotFenixesOnLevel;

Engine* Engine::engine = nullptr;
void Engine::Init()
{

	SDL_Surface* windowSurface = NULL;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0)
	{
		throw SDL_exception("SDL could not initialize!");
	}

	window = SDL_CreateWindow("Hunting Ducks 2.0", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
	windowSurface = SDL_GetWindowSurface(window);

	if (window == nullptr)
	{
		throw SDL_exception("Could not create window!");
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr)
	{
		throw SDL_exception("Could not create renderer!");
	}

	if (Mix_OpenAudio(44100, AUDIO_F32MSB, 2, 2048) < 0)
	{
		std::cout << ("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError()) << std::endl;
	}

	if (TTF_Init() < 0)
	{
		std::cout << "Error initializing SDL_ttf: " << TTF_GetError() << std::endl;
	}

	Langar = TTF_OpenFont("font/Langar-Regular.ttf", 25);
	if (Langar == nullptr)
		std::cout << TTF_GetError() << std::endl;

	running = true;
	isGameStarted = false;

	imusic = Mix_LoadMUS("sounds/Intro_music.wav");
	if (imusic == NULL)
	{
		std::cout << ("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError()) << std::endl;
	}
	gunshot = Mix_LoadWAV("sounds/gun_fire_bullet.wav");
	if (gunshot == NULL)
	{
		std::cout << ("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError()) << std::endl;
	}
	gunshot_empty = Mix_LoadWAV("sounds/gun_fire_empty.wav");
	if (gunshot_empty == NULL)
	{
		std::cout << ("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError()) << std::endl;
	}
	bird_dead = Mix_LoadWAV("sounds/level_birddeath_sound.wav");
	if (bird_dead == NULL)
	{
		std::cout << ("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError()) << std::endl;
	}
	Mix_VolumeChunk(bird_dead, MIX_MAX_VOLUME / 8);
	fly_away = Mix_LoadWAV("sounds/lostgame_music.wav");
	if (fly_away == NULL)
	{
		std::cout << ("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError()) << std::endl;
	}
	Mix_VolumeChunk(fly_away, MIX_MAX_VOLUME / 4);
	fly_in = Mix_LoadWAV("sounds/bird_emerging_sound.wav");
	if (fly_in == NULL)
	{
		std::cout << ("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError()) << std::endl;
	}

	clearFenixMap();

	auto tmpSurface = IMG_Load(background_img_path.c_str());
	backgroundTexture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(foreground_img_path.c_str());
	grassTexture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(duck_img_path.c_str());
	duckTexture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(duck_die_img_path.c_str());
	deadDuckTexture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(menu.c_str());
	menuTexture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(zero_shells.c_str());
	zero_shell_texture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(one_shell.c_str());
	one_shell_texture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(two_shells.c_str());
	two_shell_texture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(three_shells.c_str());
	three_shell_texture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(two_red_fenixes.c_str());
	two_red_fenixes_texture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(two_white_fenixes.c_str());
	two_white_fenixes_texture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(red_white_fenixes.c_str());
	red_white_fenixes_texture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(one_white_fenix.c_str());
	one_white_fenix_texture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load(one_red_fenix.c_str());
	one_red_fenix_texture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);
}

void Engine::Update()
{
	if (isGameStarted == false)
	{
		if (Mix_PlayingMusic() == 0)
		{
			//Play intro music
			Mix_PlayMusic(imusic, -1);
		}
		SDL_RenderCopy(renderer, menuTexture, nullptr, &bgRect);
		handleOnMenu();
	}
	else
	{
		Mix_FreeMusic(imusic);
		imusic = NULL;
		SDL_RenderCopy(renderer, backgroundTexture, nullptr, &bgRect);

		if (gameA == true)
		{
			if (levelCount == 9)
			{
				std::cout << "Whoopsie" << std::endl;
				cleanupBetweenGames();
				return;
			}

			if (ammoCount <= 0 || shotFenixes == 1)
			{
				timer();
				if (shotFenixes == 0)
				{
					Mix_PlayChannel(-1, fly_away, 0);
					duck1.flyAway();
				}
				cleanupBetweenLevels();
			}
			//Mix_PlayChannel( -1, fly_in, 0 );
			srand((unsigned)(time(0)));
			duck1.move();
			rect = duck1.getRect();

			int x = 0;

			Uint32 ticks = SDL_GetTicks();
			Uint32 seconds = ticks / 50;
			x = seconds % 11;

			duckFrame = { x * 500  , 0, 500 , 500 };

			if (!duck1.isAlive()) {
				SDL_RenderCopy(renderer, deadDuckTexture, NULL, rect);
			}
			else if (duck1.isFlipped())
				SDL_RenderCopyEx(renderer, duckTexture, &duckFrame, rect, 0, NULL, SDL_FLIP_HORIZONTAL);
			else
				SDL_RenderCopy(renderer, duckTexture, &duckFrame, rect);

			SDL_RenderCopy(renderer, grassTexture, nullptr, &grassRect);

			handleInGameEvents();
			renderScore();
			renderFenixesGameA();
			renderAmmo();
		}
		else if (gameB == true)
		{
			if (levelCount == 5)
			{
				cleanupBetweenGames();
				return;
			}

			if (ammoCount <= 0 || shotFenixes == 2)
			{
				timer();
				if (shotFenixes == 0)
				{
					Mix_PlayChannel(-1, fly_away, 0);
					duck1.flyAway();
					duck2.flyAway();
				}
				//gotta find a way of knowing which duck is alive (1 or 2)
				else if (shotFenixes == 1)
				{
					if (duck1.isAlive())
					{
						Mix_PlayChannel(-1, fly_away, 0);
						duck1.flyAway();
					}
					else if (duck2.isAlive())
					{
						Mix_PlayChannel(-1, fly_away, 0);
						duck2.flyAway();
					}
				}
				cleanupBetweenLevels();
			}
			//Mix_PlayChannel( -1, fly_in, 0 );
			srand((unsigned)(time(0)));
			duck1.move();
			rect = duck1.getRect();

			int x = 0;

			Uint32 ticks = SDL_GetTicks();
			Uint32 seconds = ticks / 50;
			x = seconds % 11;

			duckFrame = { x * 500  , 0, 500 , 500 };
			if (!duck1.isAlive()) {
				SDL_RenderCopy(renderer, deadDuckTexture, NULL, rect);
			}
			else if (duck1.isFlipped())
				SDL_RenderCopyEx(renderer, duckTexture, &duckFrame, rect, 0, NULL, SDL_FLIP_HORIZONTAL);
			else
				SDL_RenderCopy(renderer, duckTexture, &duckFrame, rect);

			//Mix_PlayChannel( -1, fly_in, 0 );
			srand((unsigned)(time(0) + 47));
			duck2.move();
			rect = duck2.getRect();

			if (!duck2.isAlive()) {
				SDL_RenderCopy(renderer, deadDuckTexture, NULL, rect);
			}
			else if (duck2.isFlipped())
				SDL_RenderCopyEx(renderer, duckTexture, &duckFrame, rect, 0, NULL, SDL_FLIP_HORIZONTAL);
			else
				SDL_RenderCopy(renderer, duckTexture, &duckFrame, rect);


			SDL_RenderCopy(renderer, grassTexture, nullptr, &grassRect);

			handleInGameEvents();
			renderScore();
			renderFenixesGameB();
			renderAmmo();
		}
	}
}

bool Engine::isRunning()
{
	return running;
}

void Engine::Clean()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	std::cout << "Game cleaned." << std::endl;
}

void Engine::Render()
{
	SDL_RenderPresent(renderer);
	SDL_RenderClear(renderer);
}

void Engine::handleOnMenu()
{
	SDL_Event event;
	SDL_PollEvent(&event);

	if (event.type == SDL_KEYDOWN)
	{
		switch (event.key.keysym.sym)
		{
		case SDLK_a:
			gameA = true;
			isGameStarted = true;
			break;

		case SDLK_b:
			gameB = true;
			isGameStarted = true;
		}
	}
	else if (event.type == SDL_QUIT)
		running = false;
}

void Engine::handleInGameEvents()
{
	SDL_Event event;
	SDL_PollEvent(&event);

	if (player.eventHandler(event, duck1, duck2, shotFenixes, score, bird_dead))
	{
		if (shotFenixes < 2)
			shotFenixesOnLevel[levelCount] = shotFenixesOnLevel[levelCount] + 1;
		else
			shotFenixesOnLevel[levelCount] = shotFenixes;
		std::cout << shotFenixesOnLevel[levelCount] << std::endl;
	}
	switch (event.type)
	{
	case SDL_QUIT:
		running = false;
		break;

	case SDL_MOUSEBUTTONDOWN:
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			if (ammoCount > 0)
			{
				Mix_PlayChannel(-1, gunshot, 0);
			}
			else
			{
				Mix_PlayChannel(-1, gunshot_empty, 0);
			}
			ammoCount--;
		}
		break;
	}
}

void Engine::renderAmmo()
{
	SDL_Rect tempRect = { 44, 635, 124, 75 };
	rect = &tempRect;
	switch (ammoCount)
	{
	case 3:
		SDL_RenderCopy(renderer, three_shell_texture, nullptr, rect);
		break;
	case 2:
		SDL_RenderCopy(renderer, two_shell_texture, nullptr, rect);
		break;
	case 1:
		SDL_RenderCopy(renderer, one_shell_texture, nullptr, rect);
		break;
	case 0:
		SDL_RenderCopy(renderer, zero_shell_texture, nullptr, rect);
		break;
	}
}

void Engine::renderFenixesGameA()
{
	int level = 1;
	int countOfShotFenixes = shotFenixesOnLevel[level];

	renderFenixesOnXPos(395, countOfShotFenixes, 'A');

	level++;
	countOfShotFenixes = shotFenixesOnLevel[level];
	renderFenixesOnXPos(457, countOfShotFenixes, 'A');

	level++;
	countOfShotFenixes = shotFenixesOnLevel[level];
	renderFenixesOnXPos(519, countOfShotFenixes, 'A');

	level++;
	countOfShotFenixes = shotFenixesOnLevel[level];
	renderFenixesOnXPos(581, countOfShotFenixes, 'A');

	level++;
	countOfShotFenixes = shotFenixesOnLevel[level];
	renderFenixesOnXPos(643, countOfShotFenixes, 'A');

	level++;
	countOfShotFenixes = shotFenixesOnLevel[level];
	renderFenixesOnXPos(705, countOfShotFenixes, 'A');

	level++;
	countOfShotFenixes = shotFenixesOnLevel[level];
	renderFenixesOnXPos(767, countOfShotFenixes, 'A');

	level++;
	countOfShotFenixes = shotFenixesOnLevel[level];
	renderFenixesOnXPos(829, countOfShotFenixes, 'A');
	std::cout << level << std::endl;
}

void Engine::renderFenixesGameB()
{
	int level = 1;
	int countOfShotFenixes = shotFenixesOnLevel[level];

	renderFenixesOnXPos(395, countOfShotFenixes, 'B');

	level++;
	countOfShotFenixes = shotFenixesOnLevel[level];
	renderFenixesOnXPos(519, countOfShotFenixes, 'B');

	level++;
	countOfShotFenixes = shotFenixesOnLevel[level];
	renderFenixesOnXPos(643, countOfShotFenixes, 'B');

	level++;
	countOfShotFenixes = shotFenixesOnLevel[level];
	renderFenixesOnXPos(767, countOfShotFenixes, 'B');
}

void Engine::renderFenixesOnXPos(int xPos, int count, char game)
{
	if (game == 'A')
	{
		SDL_Rect tempRect = { xPos, 640, 62, 64 };
		rect = &tempRect;
		switch (count)
		{
		case 0:
			SDL_RenderCopy(renderer, one_white_fenix_texture, nullptr, rect);
			break;
		case 1:
			SDL_RenderCopy(renderer, one_red_fenix_texture, nullptr, rect);
			break;
		}
	}
	else if (game == 'B')
	{
		SDL_Rect tempRect = { xPos, 640, 124, 64 };
		rect = &tempRect;
		switch (count)
		{
		case 0:
			SDL_RenderCopy(renderer, two_white_fenixes_texture, nullptr, rect);
			break;
		case 1:
			SDL_RenderCopy(renderer, red_white_fenixes_texture, nullptr, rect);
			break;
		case 2:
			SDL_RenderCopy(renderer, two_red_fenixes_texture, nullptr, rect);
			break;
		}
	}
}

void Engine::cleanupBetweenGames()
{
	levelCount = 1;
	isGameStarted = false;
	gameA = gameB = false;
	clearFenixMap();
}

void Engine::cleanupBetweenLevels()
{
	int duration = (clock() - start) / (double)CLOCKS_PER_SEC;

	if (duration >= 2)
	{
		levelCount++;
		ammoCount = 3;
		shotFenixes = 0;

		Mix_PlayChannel(-1, fly_in, 0);
		duck1.spawn();
		duck2.spawn();

		timerRunning = false;
	}
}

void Engine::clearFenixMap()
{
	shotFenixesOnLevel[1] = 0;
	shotFenixesOnLevel[2] = 0;
	shotFenixesOnLevel[3] = 0;
	shotFenixesOnLevel[4] = 0;
	shotFenixesOnLevel[5] = 0;
	shotFenixesOnLevel[6] = 0;
	shotFenixesOnLevel[7] = 0;
	shotFenixesOnLevel[8] = 0;
}

void Engine::timer()
{
	if (!timerRunning)
	{
		start = clock();
		timerRunning = true;
	}
}

void Engine::renderScore()
{
	std::stringstream ss;
	ss << std::setw(4) << std::setfill('0') << score;
	std::string s = ss.str();

	SDL_Surface* tempSurface = TTF_RenderText_Solid(Langar, s.c_str(), { 255, 255, 255 });
	SDL_Texture* tempTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
	if (!tempSurface)
		std::cout << TTF_GetError() << std::endl;
	SDL_Rect tempRect = { 1110, 640, 110, 70 };
	SDL_RenderCopy(renderer, tempTexture, nullptr, &tempRect);
	SDL_FreeSurface(tempSurface);
	SDL_DestroyTexture(tempTexture);
}