#pragma once
#include <random>
#include <vector>
#include <array>

#include "Vec2.h"



class Agent
{
	Vec2 m_position;
	Vec2 m_velocity;
	Vec2 m_acceleration;

	Vec2 m_enemyPosition;

public:
	float m_maxSpeed = 0.f;
	float m_neighborDistance = 0.f;
	float m_seperationDistance = 0.f;
	float m_seperateK = 0.f;
	float m_alignK = 0.f;
	float m_cohereK = 0.f;
	float m_maxForce = 0.f;

	bool m_showAlignNCohereRadius = false;
	bool m_showSeperateRadius = false;



public:
	Agent(float pos_x, float pos_y)
	{
		m_position = Vec2(pos_x, pos_y);
		generateRandomVelocity();
		m_acceleration = Vec2(0, 0);
	}

	void generateRandomVelocity()
	{
		float angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f * 3.14159f;

		// Convert the angle to a vector
		float x = cos(angle);
		float y = sin(angle);
		
		m_velocity = Vec2(x, y);
	}

	void guide(std::vector<Agent>& flock)
	{
		m_acceleration *= 0;
		steer(flock);
		enemy();
	}

	void enemy()
	{
		Vec2 diff = m_enemyPosition - m_position;
		float distance = m_enemyPosition.dist(m_position);
		if (distance > 100)
		{
			target(diff, .0008f);
		}
		if (distance < 100)
		{
			target(diff, -.005f);
		}
	}

	void steer(std::vector<Agent>& flock)
	{
		Vec2 sumSeperation = Vec2(0, 0);
		Vec2 sumCoherence = Vec2(0, 0);
		Vec2 sumAlignment = Vec2(0, 0);

		int countAlignNeighbors = 0;
		int countSeperateNeighbors = 0;

		for (auto& other : flock)
		{
			float distance = m_position.dist(other.m_position);

			// Radius of alignment and coherence area
			if ((distance > 0) && (distance < m_neighborDistance))
			{
				sumAlignment += other.m_velocity;
				sumCoherence += other.m_position;
				countAlignNeighbors++;
			}

			// Radius of separation area
			if ((distance > 0) && (distance < m_seperationDistance))
			{
				Vec2 diff = m_position - other.m_position;
				diff.normalize();
				diff /= distance;
				sumSeperation += diff;
				countSeperateNeighbors++;
			}
		}

		// Apply separation force
		if (countSeperateNeighbors > 0)
		{
			target(sumSeperation, m_seperateK);
		}

		// Apply alignment force
		if (countAlignNeighbors > 0)
		{
			target(sumAlignment, m_alignK);
			sumCoherence /= (float)countAlignNeighbors;
			sumCoherence -= m_position;
			target(sumCoherence, m_cohereK);
		}
	}

	void target(Vec2& targetDirection, float k)
	{
		targetDirection.normalize();
		targetDirection *= m_maxSpeed;
		targetDirection -= m_velocity;
		targetDirection.limit(m_maxForce);
		targetDirection *= k;
		m_acceleration += targetDirection;
	}

	void update(int windowWidth, int windowHeight)
	{
		m_velocity += m_acceleration;
		m_velocity.limit(m_maxSpeed);
		m_position += m_velocity;

		//collision resolution if agents enters the edge of window
		if (m_position.x < 0 ||
			m_position.y < 0 ||
			m_position.x > windowWidth ||
			m_position.y > windowHeight)
		{
			m_position.x = (int)(m_position.x + windowWidth) % windowWidth;
			m_position.y = (int)(m_position.y + windowHeight) % windowHeight;
		}
		
	}

	void display(sf::RenderWindow& window) const
	{
		//init the agent shape
		std::array vertices =
		{
			sf::Vertex{sf::Vector2f(8.f, 0.f)},
			sf::Vertex{sf::Vector2f(-8.f,6.f)},
			sf::Vertex{sf::Vector2f(-8.f, -6.f)}
		};

		/*vertices[0].color = sf::Color::Red;
		vertices[1].color = sf::Color::Blue;
		vertices[2].color = sf::Color::Green;*/

		vertices[0].color = sf::Color::White;
		vertices[1].color = sf::Color::White;
		vertices[2].color = sf::Color::White;
		
		sf::Transform t1;
		t1.translate({m_position.x, m_position.y});

		sf::Transform t2;
		t2.rotate(m_velocity.heading());
		//std::cout << "heading: " << m_velocity.heading() << '\n';

		sf::Transform t3 = t1 * t2;

		float center = 0;
		if (m_showSeperateRadius)
		{
			center = m_seperationDistance;
			sf::CircleShape c;
			c.setOrigin(center, center);
			c.setFillColor(sf::Color::Transparent);
			c.setOutlineColor(sf::Color::Red);
			c.setOutlineThickness(1.f);
			c.setRadius(center);
			c.setPosition(sf::Vector2f(m_position.x, m_position.y));
			window.draw(c);
		}

		if (m_showAlignNCohereRadius)
		{
			center = m_neighborDistance;
			sf::CircleShape c;
			c.setOrigin(center, center);
			c.setFillColor(sf::Color::Transparent);
			c.setOutlineColor(sf::Color::Green);
			c.setOutlineThickness(1.f);
			c.setRadius(center);
			c.setPosition(sf::Vector2f(m_position.x, m_position.y));
			window.draw(c);
		}



		window.draw(vertices.data(), vertices.size(), sf::PrimitiveType::Triangles, t3);
	}
	

	void setVariables(float maxSpeed, float neighborDistance, float seperationDistance, float seperateK, float alignK, float cohereK, float maxForce)
	{
		m_maxSpeed = maxSpeed;
		m_neighborDistance = neighborDistance;
		m_seperationDistance = seperationDistance;
		m_seperateK = seperateK;
		m_alignK = alignK;
		m_cohereK = cohereK;
		m_maxForce = maxForce;
	}

	void setShowDistance(bool showAlignNCohereRadius, bool showSeperateRadius)
	{
		m_showAlignNCohereRadius = showAlignNCohereRadius;
		m_showSeperateRadius = showSeperateRadius;
	}

	void setEnemyPosition(Vec2& position)
	{
		m_enemyPosition = position;
	}
};