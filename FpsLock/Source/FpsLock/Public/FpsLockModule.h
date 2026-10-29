#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Styling/SlateColor.h"

struct FSlateBrush;

// Editor-only toolbar button that cycles the editor frame rate cap: 30 -> 60 -> no limit.
class FFpsLockModule : public IModuleInterface
{
public:
	enum class EMode : int32 { Fps30 = 0, Fps60 = 1, Unlimited = 2 };

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterMenus();
	void SetMode(EMode NewMode, bool bSave);
	void CycleMode();

	FSlateColor GetBackgroundColor() const;
	const FSlateBrush* GetIcon() const;
	FSlateColor GetIconColor() const;
	FText GetLabel() const;
	FText GetTooltip() const;

	EMode Mode = EMode::Unlimited;
};
