#include "Player.hpp"
#include "math/Rectangle.hpp"
#include "TextureLoader.hpp"
#include <iostream>

Player::Player(SDL_Renderer* p_Renderer, const char* p_TexPath, const SDL_Rect& p_SrcRect, const int& p_MovementSpeed, const int& p_Gravity)
	: m_MovementSpeed(p_MovementSpeed), m_Gravity(p_Gravity) {
	//Load sprite and create buffer texture to render player to
	m_Sprite = TextureUtil::LoadTexture(p_TexPath, p_Renderer);
	m_Buffer = SDL_CreateTexture(p_Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 320, 180);
	SDL_SetTextureBlendMode(m_Buffer, SDL_BLENDMODE_BLEND);

	m_Renderer = p_Renderer;
	m_SrcRect = p_SrcRect;
	m_DestRect = p_SrcRect;

	m_Jumping = true;
}

Player::~Player() {
	delete m_Buffer;
	delete m_Sprite;
}

void Player::LinkCollider(DynamicCollider2D* p_Collider) {
	//Send pointers to collider
	p_Collider->LinkObject(&m_CurrVelocity, &m_CurrPosition, &m_LastPosition, &m_Jumping);
}

void Player::HandleEvents(const SDL_Event& p_Event) {
	//Primitive Input handler for now since no player controller class yet
	const Uint8* Keys = SDL_GetKeyboardState(NULL);

	//Checking for whether keys are held down or not
	if (Keys[SDL_SCANCODE_W] && !m_Jumping) {
		m_CurrVelocity.y = -140;
		m_Jumping = true;
	}
	if (Keys[SDL_SCANCODE_A]) {
		m_CurrVelocity.x = -m_MovementSpeed;
	}
	if (Keys[SDL_SCANCODE_D]) {
		m_CurrVelocity.x = m_MovementSpeed;
	}

	if (p_Event.type == SDL_KEYUP) {
		auto Key = p_Event.key.keysym.sym;

		switch (Key) {
		case SDLK_d:
			//If the opposite direction key is still pressed while it is left up, continue in the other direction
			if (Keys[SDL_SCANCODE_A]) {
				m_CurrVelocity.x = -m_MovementSpeed;
				return;
			}
			m_CurrVelocity.x = 0;
			break;
		case SDLK_a:
			if (Keys[SDL_SCANCODE_D]) {
				m_CurrVelocity.x = m_MovementSpeed;
				return;
			}
			m_CurrVelocity.x = 0;
			break;
		}
	}
}


void Player::Update(const float& p_DeltaTime) {
	//Apply gravity only while airborne
	if(m_Jumping) m_CurrVelocity.y += m_Gravity * p_DeltaTime;

	m_LastPosition = m_CurrPosition;
	m_CurrPosition = m_CurrPosition + m_CurrVelocity * p_DeltaTime; 

	m_Jumping = true;
}

void Player::Render() {
	//Setting Render Target to the buffer
	SDL_SetRenderTarget(m_Renderer, m_Buffer);
	SDL_RenderClear(m_Renderer);

	m_DestRect.x = (int)m_CurrPosition.x;
	m_DestRect.y = (int)m_CurrPosition.y;

	SDL_RenderCopy(m_Renderer, m_Sprite, &m_SrcRect, &m_DestRect);
	SDL_SetRenderTarget(m_Renderer, NULL);

	//Render the Buffer
	SDL_RenderCopy(m_Renderer, m_Buffer, NULL, NULL);
	SDL_SetRenderTarget(m_Renderer, NULL);
}