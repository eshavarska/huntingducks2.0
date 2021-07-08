#ifndef ENGINE_H
#define ENGINE_H

#include <duck/Duck.h>
#include <string>
#include <SDL.h>
#include <SDL_mixer.h>
#include <map>

class Engine
{
private:
	Engine() {}
	~Engine();

	static Engine *engine;

	SDL_Window *window;
	SDL_Renderer *renderer;
	Mix_Music *imusic = NULL;
	Mix_Chunk *gunshot = NULL;
	Mix_Chunk *gunshot_empty = NULL;
	Mix_Chunk *bird_dead = NULL;
	Mix_Chunk *fly_away = NULL;
	Mix_Chunk *fly_in = NULL;

	bool running;
	bool isGameStarted;
	bool gameA = false, gameB = false;

public:
	static Engine *getEngine()
	{
		engine = (engine == nullptr) ? new Engine() : engine;
		return engine;
	}

	void Init();
	void Update();
	bool isRunning();
	void Clean();
	void Render();
	void handleOnMenu();
	void handleInGameEvents();
	void spawnDuck(SDL_Rect *rect, SDL_Renderer *renderer);
	void renderAmmo();
	void renderFenixesOnXPos(int xPos, int count, char game);
	void renderFenixesGameB();
	void renderFenixesGameA();
	void renderScore();
	void clearFenixMap();
	void cleanupBetweenGames();
	void cleanupBetweenLevels();
	void timer();
};

#endif //ENGINE_H
