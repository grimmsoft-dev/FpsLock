#include "FpsLockModule.h"

#include "HAL/IConsoleManager.h"
#include "Misc/ConfigCacheIni.h"
#include "Modules/ModuleManager.h"
#include "Styling/AppStyle.h"
#include "ToolMenus.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "FpsLock"

namespace FpsLock
{
	static const TCHAR* ConfigSection = TEXT("FpsLock");
	static const TCHAR* ConfigKey = TEXT("Mode");
}

void FFpsLockModule::StartupModule()
{
	// Restore the last choice (saved per user, per project). "No limit" leaves t.MaxFPS untouched at startup.
	int32 Saved = (int32)EMode::Unlimited;
	GConfig->GetInt(FpsLock::ConfigSection, FpsLock::ConfigKey, Saved, GEditorPerProjectIni);
	const EMode SavedMode = (EMode)FMath::Clamp(Saved, 0, 2);
	if (SavedMode != EMode::Unlimited)
	{
		SetMode(SavedMode, false);
	}

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FFpsLockModule::RegisterMenus));
}

void FFpsLockModule::ShutdownModule()
{
	if (UObjectInitialized())
	{
		UToolMenus::UnRegisterStartupCallback(this);
		UToolMenus::UnregisterOwner(this);
	}
}

void FFpsLockModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	UToolMenu* ToolBar = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.User");
	FToolMenuSection& Section = ToolBar->FindOrAddSection("FpsLock");

	TSharedRef<SWidget> Button =
		SNew(SButton)
		.ButtonStyle(FAppStyle::Get(), "SimpleButton")
		.ContentPadding(FMargin(2.f))
		.ToolTipText_Raw(this, &FFpsLockModule::GetTooltip)
		.OnClicked_Lambda([this]() { CycleMode(); return FReply::Handled(); })
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::Get().GetBrush("WhiteBrush"))
			.BorderBackgroundColor_Raw(this, &FFpsLockModule::GetBackgroundColor)
			.Padding(FMargin(4.f, 2.f))
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(SImage)
					.Image_Raw(this, &FFpsLockModule::GetIcon)
					.ColorAndOpacity_Raw(this, &FFpsLockModule::GetIconColor)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(FMargin(3.f, 0.f, 0.f, 0.f))
				[
					SNew(STextBlock)
					.Text_Raw(this, &FFpsLockModule::GetLabel)
					.ColorAndOpacity_Raw(this, &FFpsLockModule::GetIconColor)
				]
			]
		];

	Section.AddEntry(FToolMenuEntry::InitWidget("FpsLockButton", Button, LOCTEXT("FpsLockLabel", "FPS Lock"), true));
}

void FFpsLockModule::SetMode(EMode NewMode, bool bSave)
{
	Mode = NewMode;

	const float MaxFps = Mode == EMode::Fps30 ? 30.f : Mode == EMode::Fps60 ? 60.f : 0.f;
	if (IConsoleVariable* MaxFpsVar = IConsoleManager::Get().FindConsoleVariable(TEXT("t.MaxFPS")))
	{
		MaxFpsVar->Set(MaxFps, ECVF_SetByConsole);
	}

	if (bSave)
	{
		GConfig->SetInt(FpsLock::ConfigSection, FpsLock::ConfigKey, (int32)Mode, GEditorPerProjectIni);
		GConfig->Flush(false, GEditorPerProjectIni);
	}
}

void FFpsLockModule::CycleMode()
{
	SetMode((EMode)(((int32)Mode + 1) % 3), true);
}

FSlateColor FFpsLockModule::GetBackgroundColor() const
{
	switch (Mode)
	{
	case EMode::Fps30: return FLinearColor(1.f, 0.78f, 0.f);
	case EMode::Fps60: return FLinearColor(0.15f, 0.75f, 0.2f);
	default:           return FLinearColor::Transparent;
	}
}

const FSlateBrush* FFpsLockModule::GetIcon() const
{
	return FAppStyle::Get().GetBrush(Mode == EMode::Unlimited ? "Icons.Unlock" : "Icons.Lock");
}

FSlateColor FFpsLockModule::GetIconColor() const
{
	return Mode == EMode::Unlimited ? FSlateColor::UseForeground() : FSlateColor(FLinearColor::Black);
}

FText FFpsLockModule::GetLabel() const
{
	switch (Mode)
	{
	case EMode::Fps30: return LOCTEXT("Label30", "30");
	case EMode::Fps60: return LOCTEXT("Label60", "60");
	default:           return LOCTEXT("LabelNone", "Off");
	}
}

FText FFpsLockModule::GetTooltip() const
{
	switch (Mode)
	{
	case EMode::Fps30: return LOCTEXT("Tip30", "Editor frame rate locked to 30 fps (t.MaxFPS 30).\nClick: lock to 60 fps.");
	case EMode::Fps60: return LOCTEXT("Tip60", "Editor frame rate locked to 60 fps (t.MaxFPS 60).\nClick: remove the limit.");
	default:           return LOCTEXT("TipNone", "No frame rate limit (t.MaxFPS 0).\nClick: lock to 30 fps.");
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FFpsLockModule, FpsLock)
