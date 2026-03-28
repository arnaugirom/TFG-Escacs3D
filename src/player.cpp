#include "player.h"

void Player::increaseRound()
{ 
	m_nRound++; 
	if(m_nRound < 12)
	{
		if(m_nRound % 2 == 0) m_difficulty += 0.1f;
		else m_difficulty += 0.05f;
	}
	else 
	{
		m_difficulty += (float) m_nRound / 100.0f;
	}
}

void Player::resetPlayer() 
{
	//Variables
	m_money = 5000;
	m_health = 10;
	m_nRound = 1;
	m_difficulty = 1;

	//Stats
	m_enemiesDefeated = 0;
	m_totalMoneyEarned = 0;
	m_turretsPlaced = 0;
	m_UpgradesPurchased = 0;
}