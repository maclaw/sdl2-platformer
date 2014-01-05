#include "GameStateGame.hpp"
#include "StateManager.hpp"
#include "Config.hpp"
#include "Log.hpp"

//#include "LoadingScreen.hpp"
//#include "Graphics.hpp"
//#include "PhysicsManager.hpp"

// All possible commands the user might type into the console
enum GameStateGameCommands
{
	COMMAND_QUIT,     COMMAND_ADD_PLATFORM, COMMAND_INVERT_GRAVITY,
	COMMAND_FLY,      COMMAND_ADD_CLOUD,    COMMAND_HELP,
	COMMAND_CONTROLS, COMMAND_GIVE_UP
};

GameStateGame::GameStateGame(Window* window):
	window(window),
	will_quit(false),
	bgmusic(nullptr),
	sfx(nullptr),
	gameArea(nullptr),
	player(nullptr)
{ }
GameStateGame::~GameStateGame()
{ }
void GameStateGame::load(int stack)
{
	UNUSED(stack);

	Log::log("Keybindings:");
	Log::log("p  Play/pause music");
	Log::log("+  Increase volume");
	Log::log("-  Decrease volume");
	Log::log("s  Play/pause SFX");
	Log::log("q  Quit");

	this->bgmusic = new Music("sounds/music/bg.ogg");

	this->sfx = new SFX("sounds/sfx/jump.ogg");

	this->gameArea = new Rectangle(0, 0, this->window->width, this->window->height);

	int playerW = 14;
	int playerH = 27;

	int playerX = this->gameArea->w / 2 - playerW / 2;
	int playerY = 0;

	this->player = new Player(this->window,
	                          playerX, playerY,
	                          playerW, playerH,
	                          100,
	                          Config::getInt("acceleration", 30));

	this->player->setHorizontalLimit(0, this->gameArea->w);
	this->player->setVerticalLimit(0, this->gameArea->h);
}
int GameStateGame::unload()
{
	delete this->bgmusic;
	delete this->sfx;
	delete this->player;

	return 0;
}
GameState::StateCode GameStateGame::update(float dt)
{
	if (this->will_quit)
		return GameState::QUIT;

	this->updateInput();

	if (this->player)
		this->player->update(dt);

	// Must be at the end of this function
	this->checkCollisions();

	return GameState::CONTINUE;
}
void GameStateGame::render()
{
	if (this->player)
		this->player->render();
}
void GameStateGame::checkCollisions()
{
	if (this->player)
		this->player->commitMovement();
}
void GameStateGame::updateInput()
{
	InputManager* input = InputManager::getInstance();

	input->update();

	this->will_quit = input->quitRequested();

	if ((input->isKeyDown(KEY_ESCAPE)) ||
	    (input->isKeyDown(KEY_Q)))
		this->will_quit = true;

	if (input->isKeyDown(KEY_P))
	{
		if (this->bgmusic->isPlaying())
		    this->bgmusic->pause();

		else if (this->bgmusic->isPaused())
			this->bgmusic->unpause();

		else
			this->bgmusic->play();
	}

	if ((input->isKeyDown(KEY_EQUALS) && input->shift()) ||
		(input->isKeyDown(KEY_MINUS)))
	{
		int volume = this->bgmusic->getVolume();
		int step   = 5;

		if (input->isKeyDown(KEY_EQUALS))
			this->bgmusic->setVolume(volume + step);
		else
			this->bgmusic->setVolume(volume - step);
		Log::debug("Music::volume");
	}

	if (input->isKeyDown(KEY_S))
	{
		if (this->sfx->isPlaying())
		    this->sfx->pause();

		else if (this->sfx->isPaused())
			this->sfx->unpause();

		else
			this->sfx->play();
	}

	if (input->isKeyDown(KEY_A))
	{
		this->render();
		this->window->refresh();
	}

	if (input->isKeyDown(KEY_Z))
	{
		this->window->fill(Color(255, 0, 0));
		this->window->refresh();
	}

	if (input->isKeyDown(KEY_M))
		this->window->minimize();
}

