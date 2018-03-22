//Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <iostream>

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

//Texture wrapper class
class LTexture
{
public:
	//Initializes variables
	LTexture();

	//Deallocates memory
	~LTexture();

	//Loads image at specified path
	bool loadFromFile(std::string path);

	//Deallocates texture
	void free();

	//Creates image from font string
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor);

	//Renders texture at given point
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	//Gets image dimensions
	int getWidth() { return mWidth; };
	int getHeight() { return mHeight; };

private:

	//Image dimensions
	int mWidth;
	int mHeight;
};

//The dot that will move around on the screen
class Dot
{
public:
	//The dimensions of the dot
	static const int DOT_WIDTH = 20;
	static const int DOT_HEIGHT = 20;

	//Maximum axis velocity of the dot
	static const int DOT_VEL = 10;

	//Initializes the variables
	Dot();

	//Takes key presses and adjusts the dot's velocity
	void handleEvent(SDL_Event& e);

	//Moves the dot and checks collision
	void move(SDL_Rect& wall, SDL_Rect& player1);

	//Shows the dot on the screen
	void render();

	SDL_Rect player_1;
	SDL_Rect player_2;
	SDL_Rect The_Line;
	SDL_Rect The_Ball;

	int text_x, text_y;

	float player_1_vel, player_2_vel;
	float start_vel;
	int player_1_score, player_2_score;

	bool play_start;
	bool turn;

	//The X and Y offsets of the dot
	int mPosX, mPosY;

	//The velocity of the dot
	float mVelX, mVelY;

	float Master_Vel;

	//Dot's collision box
	SDL_Rect mCollider;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Box collision detector
bool checkCollision(SDL_Rect a, SDL_Rect b);

void reset_level();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

SDL_Color textColor = { 0, 0, 0 };

//Scene textures
LTexture gDotTexture;
LTexture gTextTexture;

//The actual hardware texture
SDL_Texture * mTexture;

TTF_Font* font;

Dot dot;

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	font = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile(std::string path)
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

void LTexture::free()
{
	//Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

//void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
//{
//	//Modulate texture rgb
//	SDL_SetTextureColorMod(mTexture, red, green, blue);
//}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

//int LTexture::getWidth()
//{
//	return mWidth;
//}

//int LTexture::getHeight()
//{
//	return mHeight;
//}

Dot::Dot()
{
	//Initialize the offsets
	mPosX = 0;
	mPosY = 0;

	//Set collision box dimension
	mCollider.w = DOT_WIDTH;
	mCollider.h = DOT_HEIGHT;

	//Initialize the velocity
	mVelX = 0;
	mVelY = 0;

	Master_Vel = 5;

	player_1_vel = 0;
	player_2_vel = 0;

	start_vel = 5;

	player_1_score = 0;
	player_2_score = 0;

	text_x = 200;
	text_y = 50;

	play_start = false;
	turn = false;
}

void Dot::handleEvent(SDL_Event& e)
{
	//If a key was pressed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		std::cout << "y: " << dot.player_1.y << std::endl;
		std::cout << "h: " << dot.player_1.h + dot.player_1.y << std::endl;
		if (e.key.keysym.sym == SDLK_w)
			player_1_vel -= 5;
		if (e.key.keysym.sym == SDLK_s)
			player_1_vel += 5;
		if (e.key.keysym.sym == SDLK_UP)
			player_2_vel -= 5;
		if (e.key.keysym.sym == SDLK_DOWN)
			player_2_vel += 5;
	}
	//If a key was released
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		if (e.key.keysym.sym == SDLK_w || e.key.keysym.sym == SDLK_s)
			player_1_vel = 0;
		if (e.key.keysym.sym == SDLK_UP || e.key.keysym.sym == SDLK_DOWN)
			player_2_vel = 0;
	}
	if (e.key.keysym.sym == SDLK_SPACE)
	{
		if (!dot.play_start)
		{
			dot.mVelX = dot.start_vel;
			dot.mVelY = dot.start_vel;
		}
		dot.play_start = true;
	}
	if (e.key.keysym.sym == SDLK_ESCAPE)
	{
		exit(0);
	}
}

void Dot::move(SDL_Rect& player1, SDL_Rect& player2)
{
	//Move the dot left or right
	mPosX += mVelX;
	mCollider.x = mPosX;

	//If the dot collided or went too far to the left or right
	if ((mPosX < 0) || (mPosX + DOT_WIDTH > SCREEN_WIDTH) || checkCollision(mCollider, player1) || checkCollision(mCollider, player2))
	{
		int mid_y = mCollider.y + (mCollider.h / 2);
		float speed_value = (((Master_Vel + Master_Vel) / 2) / 3);
		if (checkCollision(mCollider, player1) || checkCollision(mCollider, player2))
		{
			if (checkCollision(mCollider, player1))
			{
				if (mCollider.x + mCollider.w >= player1.x)
				{
					dot.player_1_vel = 0;
				}
				else if (mid_y <= player1.y)
				{//15
					mPosX -= mVelX;
					mCollider.x = mPosX;
					if(mVelX <=0)
						mVelX = -speed_value;
					else
						mVelX = speed_value;
					if (mVelY <= 0)
						mVelY = 5 * -speed_value;
					else
						mVelY = 5 * speed_value;
					std::cout << 15 << std::endl;
				}
				else if ((mid_y <= player1.y + (0.15 * player1.h)))
				{//30
					mPosX -= mVelX;
					mCollider.x = mPosX;
					if (mVelX <= 0)
						mVelX = 2 * speed_value;
					else
						mVelX = 2 * -speed_value;
					if (mVelY <= 0)
						mVelY = 4 * -speed_value;
					else
						mVelY = 4 * speed_value;
					std::cout << 30 << std::endl;
				}
				else if ((mid_y <= player1.y + (0.45 * player1.h)))
				{//45
					mPosX -= mVelX;
					mCollider.x = mPosX;
					if (mVelX <= 0)
						mVelX = 3 * speed_value;
					else
						mVelX = 3 * -speed_value;
					if (mVelY <= 0)
						mVelY = 3 * -speed_value;
					else
						mVelY = 3 * speed_value;
					std::cout << 45 << std::endl;
				}
				else if ((mid_y <= player1.y + (0.55 * player1.h)))
				{//90
					mPosX -= mVelX;
					mCollider.x = mPosX;
					if (mVelX <= 0)
						mVelX = 3 * speed_value;
					else
						mVelX = 3 * -speed_value;
					mVelY = 0;
					std::cout << 90 << std::endl;
				}
				else if ((mid_y <= player1.y + (0.85 * player1.h)))
				{//45
					mPosX -= mVelX;
					mCollider.x = mPosX;
					if (mVelX <= 0)
						mVelX = 3 * speed_value;
					else
						mVelX = 3 * -speed_value;
					if (mVelY <= 0)
						mVelY = 3 * -speed_value;
					else
						mVelY = 3 * speed_value;
					std::cout << 45 << std::endl;
				}
				else if ((mid_y <= player1.y + player1.h))
				{//30
					mPosX -= mVelX;
					mCollider.x = mPosX;
					if (mVelX <= 0)
						mVelX = 2 * speed_value;
					else
						mVelX = 2 * -speed_value;
					if (mVelY <= 0)
						mVelY = 4 * -speed_value;
					else
						mVelY = 4 * speed_value;
					std::cout << 301 << std::endl;
				}
				else if ((mid_y >= player1.y + player1.h))
				{//15
					mPosX -= mVelX;
					mCollider.x = mPosX;
					if (mVelX <= 0)
						mVelX = speed_value;
					else
						mVelX = -speed_value;
					if (mVelY <= 0)
						mVelY = 5 * -speed_value;
					else
						mVelY = 5 * speed_value;
					std::cout << 15 << std::endl;
				}
			}
			if (checkCollision(mCollider, player2))
			{
				if (mCollider.x + mCollider.w >= player2.x)
				{
					dot.player_2_vel = 0;
				}
				else if (mid_y <= player2.y)
				{//15
					mPosX -= mVelX;
					mCollider.x = mPosX;
					if (mVelX <= 0)
						mVelX = speed_value;
					else
						mVelX = -speed_value;
					if (mVelY <= 0)
						mVelY = 5 * -speed_value;
					else
						mVelY = 5 * speed_value;
					std::cout << 15 << std::endl;
				}
				else if ((mid_y <= player2.y + (0.15 * player2.h)))
				{//30
					mPosX -= mVelX;
					mCollider.x = mPosX;
					if (mVelX <= 0)
						mVelX = 2 * speed_value;
					else
						mVelX = 2 * -speed_value;
					if (mVelY <= 0)
						mVelY = 4 * -speed_value;
					else
						mVelY = 4 * speed_value;
					std::cout << 30 << std::endl;
				}
				else if ((mid_y <= player2.y + (0.45 * player2.h)))
				{//45
					mPosX -= mVelX;
					mCollider.x = mPosX;
					if (mVelX <= 0)
						mVelX = 3 * speed_value;
					else
						mVelX = 3 * -speed_value;
					if (mVelY <= 0)
						mVelY = 3 * -speed_value;
					else
						mVelY = 3 * speed_value;
					std::cout << 45 << std::endl;
				}
				else if ((mid_y <= player2.y + (0.55 * player2.h)))
				{//90
					mPosX -= mVelX;
					mCollider.x = mPosX;
					if (mVelX <= 0)
						mVelX = 3 * speed_value;
					else
						mVelX = 3 * -speed_value;
					mVelY = 0;
					std::cout << 90 << std::endl;
				}
				else if ((mid_y <= player2.y + (0.85 * player2.h)))
				{//45
					mPosX -= mVelX;
					mCollider.x = mPosX;
					if (mVelX <= 0)
						mVelX = 3 * speed_value;
					else
						mVelX = 3 * -speed_value;
					if (mVelY <= 0)
						mVelY = 3 * -speed_value;
					else
						mVelY = 3 * speed_value;
					std::cout << 45 << std::endl;
				}
				else if ((mid_y <= player2.y + player2.h))
				{//30
					mPosX -= mVelX;
					mCollider.x = mPosX;
					if (mVelX <= 0)
						mVelX = 2 * speed_value;
					else
						mVelX = 2 * -speed_value;
					if (mVelY <= 0)
						mVelY = 4 * -speed_value;
					else
						mVelY = 4 * speed_value;
					std::cout << 301 << std::endl;
				}
				else if ((mid_y >= player2.y + player2.h))
				{//15
					mPosX -= mVelX;
					mCollider.x = mPosX;
					if (mVelX <= 0)
						mVelX = speed_value;
					else
						mVelX = -speed_value;
					if (mVelY <= 0)
						mVelY = 5 * -speed_value;
					else
						mVelY = 5 * speed_value;
					std::cout << 15 << std::endl;
				}
			}
			Master_Vel += 0.05;
		}
		else if ((mPosX < 0))
		{
			//Move back
			mPosX -= mVelX;
			mCollider.x = mPosX;
			mVelX = -mVelX;
			player_2_score++;
			reset_level();
			std::cout << "the left wall" << std::endl;
		}
		else if((mPosX + DOT_WIDTH > SCREEN_WIDTH))
		{
			//Move back
			mPosX -= mVelX;
			mCollider.x = mPosX;
			mVelX = -mVelX;
			player_1_score++;
			reset_level();
			std::cout << "the right wall" << std::endl;
		}
	}

	//Move the dot up or down
	mPosY += mVelY;
	mCollider.y = mPosY;

	//If the dot collided or went too far up or down
	if ((mPosY < 0) || (mPosY + DOT_HEIGHT > SCREEN_HEIGHT) || checkCollision(mCollider, player1) || checkCollision(mCollider, player2))
	{
		//Move back
		mPosY -= mVelY;
		mCollider.y = mPosY;
		mVelY = -mVelY;
	}
}

void Dot::render()
{
	//Show the dot
	gTextTexture.render(text_x, text_y);
}

bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, textureText.c_str(), textColor);
	if (textSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}

	//Return success
	return mTexture != NULL;
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}
		TTF_Init();
		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load press texture
	if (!gDotTexture.loadFromFile("dot.bmp"))
	{
		printf("Failed to load dot texture!\n");
		success = false;
	}
	//Open the font
	font = TTF_OpenFont("C:/Users/IN3LABS/source/repos/Project1/Debug/AbhayaLibre.ttf", 50);
	if (font == NULL)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	else
	{
		//Render text
		if (!gTextTexture.loadFromRenderedText("Player 1: " + dot.player_1_score, textColor))
		{
			printf("Failed to render text texture!\n");
			success = false;
		}
	}

	return success;
}

void close()
{
	//Free loaded images
	gDotTexture.free();

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

bool checkCollision(SDL_Rect a, SDL_Rect b)
{
	//The sides of the rectangles
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	//Calculate the sides of rect A
	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	//Calculate the sides of rect B
	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y + b.h;

	//If any of the sides from A are outside of B
	if (bottomA <= topB)
	{
		return false;
	}

	if (topA >= bottomB)
	{
		return false;
	}

	if (rightA <= leftB)
	{
		return false;
	}

	if (leftA >= rightB)
	{
		return false;
	}

	//If none of the sides from A are outside B
	return true;
}

void reset_level()
{
	dot.player_1.y = 250;
	dot.player_2.y = 250;
	dot.player_1_vel = 0;
	dot.player_2_vel = 0;

	if (dot.turn = false)
	{
		dot.mPosX = dot.player_1.x + dot.player_1.w;
		dot.mPosY = dot.player_1.y + (dot.player_1.h / 2) - 10;
		dot.turn = true;
	}
	else
	{
		dot.mPosX = dot.player_2.x - dot.The_Ball.w;
		dot.mPosY = dot.player_2.y + (dot.player_2.h / 2) - 10;
		dot.turn = false;
	}
	dot.play_start = false;
}

void update()
{
	dot.move(dot.player_1, dot.player_2);

	if (dot.player_1.y <= 0 && dot.player_1_vel < 0)
		dot.player_1_vel = 0;
	if (dot.player_1.y + dot.player_1.h >= SCREEN_HEIGHT && dot.player_1_vel > 0)
		dot.player_1_vel = 0;
	if (dot.player_2.y <= 0 && dot.player_2_vel < 0)
		dot.player_2_vel = 0;
	if (dot.player_2.y + dot.player_2.h >= SCREEN_HEIGHT && dot.player_2_vel > 0)
		dot.player_2_vel = 0;

	dot.player_1.y += dot.player_1_vel;
	dot.player_2.y += dot.player_2_vel;

	if (!dot.play_start)
	{
		float dot_x = dot.player_1.x + dot.player_1.w;
		float dot_y = dot.player_1.y + (dot.player_1.h / 2) - 10;

		dot.mPosX = dot_x;
		dot.mPosY = dot_y;
	}

	
	gTextTexture.free();
	gTextTexture.loadFromRenderedText(std::to_string(dot.player_1_score) + "                                   " + std::to_string(dot.player_2_score), textColor);

	dot.The_Ball.x = dot.mPosX;
	dot.The_Ball.y = dot.mPosY;
}

void render()
{
	//Clear screen
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);

	//Render wall
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderDrawRect(gRenderer, &dot.player_1);

	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderDrawRect(gRenderer, &dot.player_2);

	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderDrawRect(gRenderer, &dot.The_Line);

	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderDrawRect(gRenderer, &dot.The_Ball);

	//Render dot
	dot.render();

	//Update screen
	SDL_RenderPresent(gRenderer);
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//The dot that will be moving around on the screen
			dot.player_1.x = 50;
			dot.player_1.y = 250;
			dot.player_1.w = 20;
			dot.player_1.h = 80;

			dot.player_2.x = 730;
			dot.player_2.y = 250;
			dot.player_2.w = 20;
			dot.player_2.h = 80;

			dot.The_Line.x = SCREEN_WIDTH / 2;
			dot.The_Line.y = 0;
			dot.The_Line.w = 0;
			dot.The_Line.h = SCREEN_HEIGHT;

			dot.The_Ball.x = dot.player_1.x + dot.player_1.w;
			dot.The_Ball.y = dot.player_1.y + (dot.player_1.h / 2) - 10;
			dot.The_Ball.w = 20;
			dot.The_Ball.h = 20;

			//While application is running
			while (!quit)
			{
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}

					//Handle input for the dot
					dot.handleEvent(e);
				}
				
				update();
				render();
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}