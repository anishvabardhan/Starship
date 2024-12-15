#include "Game/App.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Window/Window.hpp"

#include "Game/Game.hpp"

App* g_theApp = nullptr;
Renderer* g_theRenderer = nullptr;
AudioSystem* g_theAudio = nullptr;
Window* g_theWindow = nullptr;

App::App()
{
}

App::~App()
{
}

void App::StartUp()
{
	//m_previousFrameTime = GetCurrentTimeSeconds();

	EventSystemConfig eventSystemConfig;
	g_theEventSystem = new EventSystem(eventSystemConfig);

	DevConsoleConfig consoleConfig;
	consoleConfig.m_fontFilePath = "Data/Textures/SquirrelFixedFont.png";
	g_theConsole = new DevConsole(consoleConfig);

	InputConfig inputConfig;
	g_theInputSystem = new InputSystem(inputConfig);

	WindowConfig windowConfig;
	windowConfig.m_inputSystem = g_theInputSystem;
	windowConfig.m_windowTitle = "SD1-A4: Starship Gold";
	windowConfig.m_clientAspect = 2.0f;
	g_theWindow = new Window(windowConfig);

	RenderConfig renderConfig;
	renderConfig.m_window = g_theWindow;
	g_theRenderer = new Renderer(renderConfig);

	g_theAudio = new AudioSystem();
	m_theGame = new Game();

	g_theEventSystem->StartUp();
	g_theConsole->StartUp();
	g_theInputSystem->StartUp();
	g_theWindow->StartUp();
	g_theRenderer->StartUp();
	g_theAudio->Startup();
	m_theGame->StartUp();

	SubscribeEventCallbackFunction("QUIT", App::QuitApp);
}

void App::Run()
{
	while (!IsQuitting())
	{
		RunFrame();
	}
}

void App::ShutDown()
{
	m_theGame->Shutdown();
	g_theAudio->Shutdown();
	g_theRenderer->ShutDown();
	g_theWindow->ShutDown();
	g_theInputSystem->ShutDown();
	g_theConsole->ShutDown();
	g_theEventSystem->ShutDown();

	DELETE_PTR(m_theGame);
	DELETE_PTR(g_theRenderer);
	DELETE_PTR(g_theAudio);
	DELETE_PTR(g_theWindow);
	DELETE_PTR(g_theInputSystem);
}

void App::RunFrame()
{
	Clock::TickSystemClock();

	float deltaSeconds = Clock::GetSystemClock().GetDeltaSeconds();

	BeginFrame();
	Update(deltaSeconds);
	Render();
	EndFrame();
}

void App::BeginFrame()
{
	g_theEventSystem->BeginFrame();
	g_theConsole->BeginFrame();
	g_theWindow->BeginFrame();
	g_theInputSystem->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theAudio->BeginFrame();
}

void App::Update(float deltaseconds)
{
	if (!g_theConsole->IsOpen())
	{
		if (g_theInputSystem->WasKeyJustPressed('P'))
		{
			m_isPaused = !m_isPaused;
		}

		if (g_theInputSystem->WasKeyJustPressed('O'))
		{
			m_theGame->Update(deltaseconds);
			m_isPaused = true;
		}

		if (g_theInputSystem->WasKeyJustPressed('T'))
		{
			m_isSlowMo = true;
		}
		if (g_theInputSystem->WasKeyJustReleased('T'))
		{
			m_isSlowMo = false;
		}
	}

	if (g_theInputSystem->WasKeyJustPressed(KEYCODE_TILDE))
	{
		g_theConsole->ToggleOpen();
	}

	if (g_theInputSystem->WasKeyJustPressed(119))
	{
		m_theGame->Shutdown();
		delete m_theGame;
		m_theGame = new Game();
		m_theGame->StartUp();
	}

	if (g_theInputSystem->WasKeyJustPressed(' ') && m_theGame->m_numOfPlayerLives < 0 && m_theGame->m_isAttractMode)
	{
		m_theGame->Shutdown();
		delete m_theGame;
		m_theGame = new Game();
		m_theGame->StartUp();
	}

	if (m_isPaused && !m_isSlowMo)
	{
		m_theGame->Update(0.0f);
	}
	else if(!m_isPaused && m_isSlowMo)
	{
		m_theGame->Update(deltaseconds * 0.1f);
	}
	else if(!m_isPaused && !m_isSlowMo)
	{
		m_theGame->Update(deltaseconds);
	}
}

void App::Render() const
{
	g_theRenderer->ClearScreen(Rgba8(0, 0, 0, 255));

	m_theGame->Render();

	if (g_theConsole->IsOpen())
	{
		g_theRenderer->BeginCamera(*m_theGame->m_screenCamera, RootSig::DEFAULT_PIPELINE);

		g_theConsole->Render(AABB2(m_theGame->m_screenCamera->GetOrthoBottomLeft(), m_theGame->m_screenCamera->GetOrthoTopRight()), g_theRenderer);

		g_theRenderer->EndCamera(*m_theGame->m_screenCamera);
	}
}

void App::EndFrame()
{
	g_theAudio->EndFrame();
	g_theRenderer->EndFrame();
	g_theWindow->EndFrame();
	g_theInputSystem->EndFrame();
	g_theConsole->EndFrame();
	g_theEventSystem->EndFrame();
}

void App::HandleKeyPressed(unsigned char keyCode)
{
	g_theInputSystem->HandleKeyPressed(keyCode);
}

void App::HandleKeyReleased(unsigned char keyCode)
{
	g_theInputSystem->HandleKeyReleased(keyCode);
}

void App::HandleQuitRequested()
{
	m_isQuitting = true;
}

float App::GetDeltaTime() const
{
	return m_deltaTime;
}

void App::SetDeltaTime(float newDeltaTime)
{
	m_deltaTime = newDeltaTime;
}

bool App::QuitApp(EventArgs& args)
{
	UNUSED(args);

	g_theApp->HandleQuitRequested();

	return true;
}
