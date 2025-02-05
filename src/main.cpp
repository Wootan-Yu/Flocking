#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <random>
#include "Agent.hpp"


#pragma region imgui
#include "imgui.h"
#include "imgui-SFML.h"
#include "imguiThemes.h"
#pragma endregion

//flocking
// 1. seperation
// there is a certain radius when agents close to each other, will be seperated.
// idea: get the 'average position'(seperation) of agents that wants to be avoided then calculate the 'vector
//		 to the average position' to get the opposite direction 
// 
// 2. cohesion
// another bigger radius for the general set of neighbors
// idea: get the 'average position of the neighbors'(cohesion) then steer towards them 
//		 to balance it
// 
// 3. alignment
// idea: get the 'average heading'(alignment) of all the neighbors then put it in as vector 
//		 then steer from that direction


constexpr const short FLOCK_LENGTH = 50;

static int generateRandomPosition(int windowSize);

int main()
{
	sf::RenderWindow window(sf::VideoMode(1600,900), "Boids");
	window.setFramerateLimit(60);




	std::vector<Agent> flock;
	flock.reserve(FLOCK_LENGTH);

	for(size_t i = 0; i < FLOCK_LENGTH; i++)
	{ 
		//std::cout << generateRandomPosition(window.getSize().x) << ", " << generateRandomPosition(window.getSize().y) << '\n';
		Agent agent(generateRandomPosition(window.getSize().x), generateRandomPosition(window.getSize().y));
		flock.push_back(agent);
	}


	std::cout << "size: " << flock.size() << '\n';

	float maxSpeed = 1.f;
	float neighborDistance = 100.f;
	float seperationDistance = 50.f;
	float seperateK = 0.18f;
	float alignK = .006f;
	float cohereK = .0005f;
	float maxForce = .002f;

	bool showAC = false;
	bool showS = false;

#pragma region imgui
	ImGui::SFML::Init(window);
	//you can use whatever imgui theme you like!
	//ImGui::StyleColorsDark();				
	//imguiThemes::yellow();
	//imguiThemes::gray();
	imguiThemes::green();
	//imguiThemes::red();
	//imguiThemes::gray();
	//imguiThemes::embraceTheDarkness();

	ImGuiIO &io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.FontGlobalScale = 1.3f;
	ImGuiStyle &style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg].w = 0.5f;
	//style.Colors[ImGuiCol_DockingEmptyBg].w = 0.f;
#pragma endregion
	bool leftMouseButtonDown = false;
	Vec2 mousePos = Vec2(0.f, 0.f);

	sf::Clock clock;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
		#pragma region imgui
			ImGui::SFML::ProcessEvent(window, event);
		#pragma endregion

			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::Resized)
			{
				// Adjust the viewport when the window is resized
				sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				window.setView(sf::View(visibleArea));
			}

			mousePos.x = sf::Mouse::getPosition(window).x;
			mousePos.y = sf::Mouse::getPosition(window).y;
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (!leftMouseButtonDown)
				{
					leftMouseButtonDown = true;
				}
			}

			if (event.type == sf::Event::MouseButtonReleased)
			{
				if (leftMouseButtonDown)
				{
					//std::cout << "x: " << mousePos.x << " y: " << mousePos.y << '\n';
					leftMouseButtonDown = false;

				}
			}
		}

		//calculate the delta time
		sf::Time deltaTime = clock.restart();
		float deltaTimeSeconds = deltaTime.asSeconds();

		//make sure delta time stays within normal bounds, like between one FPS and zero FPS
		deltaTimeSeconds = std::min(deltaTimeSeconds, 1.f);
		deltaTimeSeconds = std::max(deltaTimeSeconds, 0.f);

	#pragma region imgui
		ImGui::SFML::Update(window, deltaTime);

		ImGui::PushStyleColor(ImGuiCol_WindowBg, {});
		ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, {});
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		ImGui::PopStyleColor(2);
	#pragma endregion



		ImGui::Begin("Controls");
		ImGui::SliderFloat("Max Speed", &maxSpeed, 0.f, 2.f, "%.1f");
		ImGui::SliderFloat("Neighbor Distance", &neighborDistance, 0.f, 150.f, "%.0f");
		ImGui::SliderFloat("Seperation Distance", &seperationDistance, 0.f, 100.f, "%.0f");
		ImGui::SliderFloat("Seperate K", &seperateK, 0.01f, 0.9f, "%.2f");
		ImGui::SliderFloat("Align K", &alignK, 0.001f, 0.09f, "%.3f");
		ImGui::SliderFloat("Cohere K", &cohereK, 0.0001f, 0.009f, "%.3f");
		ImGui::SliderFloat("Max Force", &maxForce, 0.0001f, 0.09f, "%.3f");
		
		bool isDefault = ImGui::Button("Default");
		ImGui::SameLine();
		bool isReset = ImGui::Button("Reset");

		ImGui::Checkbox("show Align & Cohere Radius", &showAC);
		ImGui::Checkbox("show Seperate Radius", &showS);

		ImGui::End();



		if (isDefault)
		{
				maxSpeed = 1.f;
				neighborDistance = 100.f;
				seperationDistance = 50.f;
				seperateK = 0.18f;
				alignK = .008f;
				cohereK = .0001f;
				maxForce = .002f;
		}

		if (isReset)
		{
			maxSpeed = 0.f;
			neighborDistance = 0.f;
			seperationDistance = 0.f;
			seperateK = 0.f;
			alignK = 0.f;
			cohereK = 0.f;
			maxForce = 0.f;
		}

		//game code....
		window.clear();
		//window.draw(shape);
		for (Agent& a : flock)
		{
			a.setVariables(maxSpeed, neighborDistance, seperationDistance, seperateK, alignK, cohereK, maxForce);
			a.setShowDistance(showAC, showS);
			a.setEnemyPosition(mousePos);
			a.guide(flock);
			a.update(window.getSize().x, window.getSize().y);
			a.display(window);
		}

	#pragma region imgui
		ImGui::SFML::Render(window);
	#pragma endregion

		window.display();
	}

#pragma region imgui
	ImGui::SFML::Shutdown();
#pragma endregion

	return 0;
}



int generateRandomPosition(int windowSize)
{
	std::random_device rd;
	std::mt19937 gen(rd());

	//uniform distribution in range(0, windowSize)
	std::uniform_real_distribution<float> distribution(0.f, windowSize);

	int newPos = distribution(gen);

	return newPos;
}

