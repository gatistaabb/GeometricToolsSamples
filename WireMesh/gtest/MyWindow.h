void InitWindow()
{
#if defined(_DEBUG)
	LogReporter reporter(
		"LogReport.txt",
		Logger::Listener::LISTEN_FOR_ALL,
		Logger::Listener::LISTEN_FOR_ALL,
		Logger::Listener::LISTEN_FOR_ALL,
		Logger::Listener::LISTEN_FOR_ALL);
#endif

	Window::Parameters parameters(L"gtest", 0, 0, 512, 512);
	auto window = TheWindowSystem.Create<gtest>(parameters);
	TheWindowSystem.MessagePump(window, TheWindowSystem.DEFAULT_ACTION);
	TheWindowSystem.Destroy(window);
}