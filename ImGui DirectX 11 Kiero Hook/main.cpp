#include "includes.h"
#include "font.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);

	ImGuiStyle* style = &ImGui::GetStyle();
	style->GrabRounding = 4.0f;

	ImVec4* colors = style->Colors;
	colors[ImGuiCol_Text] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY800); // text on hovered controls is gray900
	colors[ImGuiCol_TextDisabled] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY500);
	colors[ImGuiCol_WindowBg] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY100);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY50); // not sure about this. Note: applies to tooltips too.
	colors[ImGuiCol_Border] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY300);
	colors[ImGuiCol_BorderShadow] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::Static::NONE); // We don't want shadows. Ever.
	colors[ImGuiCol_FrameBg] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY75); // this isnt right, spectrum does not do this, but it's a good fallback
	colors[ImGuiCol_FrameBgHovered] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY50);
	colors[ImGuiCol_FrameBgActive] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY200);
	colors[ImGuiCol_TitleBg] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY300); // those titlebar values are totally made up, spectrum does not have this.
	colors[ImGuiCol_TitleBgActive] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY200);
	colors[ImGuiCol_TitleBgCollapsed] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY400);
	colors[ImGuiCol_MenuBarBg] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY100);
	colors[ImGuiCol_ScrollbarBg] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY100); // same as regular background
	colors[ImGuiCol_ScrollbarGrab] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY400);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY600);
	colors[ImGuiCol_ScrollbarGrabActive] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY700);
	colors[ImGuiCol_CheckMark] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::BLUE500);
	colors[ImGuiCol_SliderGrab] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY700);
	colors[ImGuiCol_SliderGrabActive] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY800);
	colors[ImGuiCol_Button] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY75); // match default button to Spectrum's 'Action Button'.
	colors[ImGuiCol_ButtonHovered] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY50);
	colors[ImGuiCol_ButtonActive] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY200);
	colors[ImGuiCol_Header] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::BLUE400);
	colors[ImGuiCol_HeaderHovered] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::BLUE500);
	colors[ImGuiCol_HeaderActive] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::BLUE600);
	colors[ImGuiCol_Separator] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY400);
	colors[ImGuiCol_SeparatorHovered] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY600);
	colors[ImGuiCol_SeparatorActive] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY700);
	colors[ImGuiCol_ResizeGrip] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY400);
	colors[ImGuiCol_ResizeGripHovered] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY600);
	colors[ImGuiCol_ResizeGripActive] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::GRAY700);
	colors[ImGuiCol_PlotLines] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::BLUE400);
	colors[ImGuiCol_PlotLinesHovered] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::BLUE600);
	colors[ImGuiCol_PlotHistogram] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::BLUE400);
	colors[ImGuiCol_PlotHistogramHovered] = ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::BLUE600);
	colors[ImGuiCol_TextSelectedBg] = ImGui::ColorConvertU32ToFloat4((ImGui::Spectrum::BLUE400 & 0x00FFFFFF) | 0x33000000);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImGui::ColorConvertU32ToFloat4((ImGui::Spectrum::GRAY900 & 0x00FFFFFF) | 0x0A000000);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

	ImFontConfig font;
	font.FontDataOwnedByAtlas = false;

	io.Fonts->AddFontFromMemoryTTF((void*)rawData, sizeof(rawData), 18.5f, &font);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool init = false;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
			init = true;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}

	if (GetAsyncKeyState(VK_INSERT) & 1) {
		globals::open = !globals::open;
	}

	if (globals::open) {
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowSize(ImVec2(450, 300));

		ImGui::Begin(("Schedule 1 - 11xy on discord"), NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		{
			ImGui::SameLine();

			if (ImGui::Button(("Main"), ImVec2(215, 20))) {
				globals::whichTab = 0;
			}
			ImGui::SameLine();

			if (ImGui::Button(("Menu shit"), ImVec2(215, 20))) {
				globals::whichTab = 1;
			}

			ImGui::Separator();

			if (globals::whichTab == 0) {
				ImGui::Text("!!MAKE SURE YOU ARE IN-GAME BEFORE YOU CLICK ANYTHING!!");
				ImGui::Text("Cash stuff");
				ImGui::Separator();

				if (ImGui::InputFloat((""), &globals::cash)) {
					
				}
				ImGui::SameLine();

				if (ImGui::Button(("Set cash"))) {
					*(float*)mem::FindAddress(moduleBase + 0x037976F8, { 0xB8 , 0x10 , 0x108 , 0x38 }) = globals::cash;
				}
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
					ImGui::SetTooltip("Cash will show in-game when something is done to it. (left or right click on it)");
				}

				ImGui::Text("ATM stuff");
				ImGui::Separator();

				if (ImGui::Button(("Reset ATM Limit"))) {
					*(float*)mem::FindAddress(moduleBase + 0x03772268, { 0x9B8 , 0xF18 , 0x6F0 , 0x510 }) = 0.0f;
				}
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
					ImGui::SetTooltip("Will set the weekly limit on the ATM to 0.");
				}

				if (ImGui::InputFloat((" "), &globals::atminput)) {

				}
				ImGui::SameLine();

				if (ImGui::Button(("Set deposit"))) {
					*(float*)mem::FindAddress(moduleBase + 0x03849880, { 0xB8 , 0x0 , 0x48 , 0x978 }) = globals::atminput;
				}
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
					ImGui::SetTooltip("This will set how much you will put into your bank from your cash. (This bypasses the $10000 limit)");
				}
				ImGui::SameLine();

				if (ImGui::Button(("Max"))) {
					*(float*)mem::FindAddress(moduleBase + 0x03849880, { 0xB8 , 0x0 , 0x48 , 0x978 }) = *(float*)mem::FindAddress(moduleBase + 0x037976F8, { 0xB8 , 0x10 , 0x108 , 0x38 });
				}
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
					ImGui::SetTooltip("This will set the deposit amount to how much cash you have.");
				}
			}
			if (globals::whichTab == 1) {
				if (ImGui::Button("Unhook")) {
					kiero::shutdown();
				}
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
					ImGui::SetTooltip("Removes hook from game and allows reinjection.");
				}

				ImGui::Text("By bettu (11xy on discord)");
			}
		}

		ImGui::End();

		ImGui::Render();

		pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	}
	return oPresent(pSwapChain, SyncInterval, Flags);
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)& oPresent, hkPresent);
			init_hook = true;
		}
	} while (!init_hook);
	return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hMod);
		CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		kiero::shutdown();
		break;
	}
	return TRUE;
}