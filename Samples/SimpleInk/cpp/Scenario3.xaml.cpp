//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "Scenario3.xaml.h"
#include <WindowsNumerics.h>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Input::Inking;
using namespace Windows::UI::Core;
using namespace Windows::Foundation::Numerics;


Scenario3::Scenario3() : rootPage(MainPage::Current)

{
    InitializeComponent();

	penSize = minPenSize + penSizeIncrement * PenThickness->SelectedIndex;

	InkDrawingAttributes^ drawingAttributes = ref new InkDrawingAttributes();
	drawingAttributes->Color = Windows::UI::Colors::Red;
	drawingAttributes->Size = Size((float)penSize, (float)penSize);
	drawingAttributes->IgnorePressure = false;
	drawingAttributes->FitToCurve = true;

	inkCanvas->InkPresenter->UpdateDefaultDrawingAttributes(drawingAttributes);
	inkCanvas->InkPresenter->InputDeviceTypes = CoreInputDeviceTypes::Mouse | CoreInputDeviceTypes::Pen;
	inkCanvas->InkPresenter->StrokesCollected += ref new TypedEventHandler<InkPresenter^, InkStrokesCollectedEventArgs^>(this, &Scenario3::InkPresenter_StrokesCollected);
	inkCanvas->InkPresenter->StrokesErased += ref new TypedEventHandler<InkPresenter^, InkStrokesErasedEventArgs^>(this, &Scenario3::InkPresenter_StrokesErased);
}


void Scenario3::InkPresenter_StrokesErased(InkPresenter^ sender, InkStrokesErasedEventArgs^ args)
{
	rootPage->NotifyUser(args->Strokes->Size + " stroke(s) erased!", NotifyType::StatusMessage);
}

void Scenario3::InkPresenter_StrokesCollected(InkPresenter^ sender, InkStrokesCollectedEventArgs^ args)
{
	rootPage->NotifyUser(args->Strokes->Size + " stroke(s) collected!", NotifyType::StatusMessage);
}


void Scenario3::OnSizeChanged(Platform::Object^ sender, SizeChangedEventArgs e)
{
	HelperFunctions::UpdateCanvasSize(RootGrid, outputGrid, inkCanvas);
}

void Scenario3::OnPenColorChanged(Platform::Object^ sender, RoutedEventArgs^ e)
{
	if (inkCanvas != nullptr)
	{
		InkDrawingAttributes^ drawingAttributes = inkCanvas->InkPresenter->CopyDefaultDrawingAttributes();

		// Use button's background to set new pen's color
		Button^ btnSender = (Button^)sender;
		Media::SolidColorBrush^ brush = (Media::SolidColorBrush^)btnSender->Background;

		drawingAttributes->Color = brush->Color;
		inkCanvas->InkPresenter->UpdateDefaultDrawingAttributes(drawingAttributes);
	}
}

void Scenario3::OnPenThicknessChanged(Platform::Object^ sender, RoutedEventArgs^ e)
{
	if (inkCanvas != nullptr)
	{
		InkDrawingAttributes^ drawingAttributes = inkCanvas->InkPresenter->CopyDefaultDrawingAttributes();
		penSize = minPenSize + penSizeIncrement * PenThickness->SelectedIndex;
		Platform::String^ value = ((ComboBoxItem^)PenType->SelectedItem)->Content->ToString();
		if (value == "Highlighter" || value == "Calligraphy")
		{
			// Make the pen tip rectangular for highlighter and calligraphy pen
			drawingAttributes->Size = Size((float)penSize, (float)(penSize * 2));
		}
		else
		{
			// Otherwise, use a square pen tim.
			drawingAttributes->Size = Size((float)penSize, (float)penSize);
		}
		inkCanvas->InkPresenter->UpdateDefaultDrawingAttributes(drawingAttributes);
	}
}

void Scenario3::OnPenTypeChanged(Platform::Object^ sender, RoutedEventArgs^ e)
{
	if (inkCanvas != nullptr)
	{
		InkDrawingAttributes^ drawingAttributes = inkCanvas->InkPresenter->CopyDefaultDrawingAttributes();
		Platform::String^ value = ((ComboBoxItem^)PenType->SelectedItem)->Content->ToString();

		if (value == "Ballpoint")
		{
			if (drawingAttributes->Kind != InkDrawingAttributesKind::Default)
			{
				auto newDrawingAttributes = ref new InkDrawingAttributes();
				newDrawingAttributes->Color = drawingAttributes->Color;
				drawingAttributes = newDrawingAttributes;
			}
			drawingAttributes->Size = Size((float)penSize, (float)penSize);
			drawingAttributes->PenTip = PenTipShape::Circle;
			drawingAttributes->DrawAsHighlighter = false;
			drawingAttributes->PenTipTransform = float3x2::identity();
		}
		else if (value == "Highlighter")
		{
			if (drawingAttributes->Kind != InkDrawingAttributesKind::Default)
			{
				auto newDrawingAttributes = ref new InkDrawingAttributes();
				newDrawingAttributes->Color = drawingAttributes->Color;
				drawingAttributes = newDrawingAttributes;
			}
			// Make the pen rectangular for highlighter
			drawingAttributes->Size = Size((float)penSize, (float)(penSize * 2));
			drawingAttributes->PenTip = PenTipShape::Rectangle;
			drawingAttributes->DrawAsHighlighter = true;
			drawingAttributes->PenTipTransform = float3x2::identity();
		}
		else if (value == "Calligraphy")
		{
			if (drawingAttributes->Kind != InkDrawingAttributesKind::Default)
			{
				auto newDrawingAttributes = ref new InkDrawingAttributes();
				newDrawingAttributes->Color = drawingAttributes->Color;
				drawingAttributes = newDrawingAttributes;
			}
			drawingAttributes->Size = Size((float)penSize, (float)(penSize * 2));
			drawingAttributes->PenTip = PenTipShape::Rectangle;
			drawingAttributes->DrawAsHighlighter = false;

			// Set a 45 degree rotation on the pen tip
			double radians = 45.0 * M_PI / 180.0;
			drawingAttributes->PenTipTransform = make_float3x2_rotation((float)radians, float2::zero());
		}
		else if (value == "Windows.UI.Xaml.Controls.TextBlock")
		{
			if (drawingAttributes->Kind != InkDrawingAttributesKind::Pencil)
			{
				auto newDrawingAttributes = InkDrawingAttributes::CreateForPencil();
				newDrawingAttributes->Color = drawingAttributes->Color;
				newDrawingAttributes->Size = drawingAttributes->Size;
				drawingAttributes = newDrawingAttributes;
			}
		}
		inkCanvas->InkPresenter->UpdateDefaultDrawingAttributes(drawingAttributes);
	}
}

void Scenario3::OnClear(Platform::Object^ sender, RoutedEventArgs^ e)
{
	inkCanvas->InkPresenter->StrokeContainer->Clear();
	rootPage->NotifyUser("Cleared Canvas", NotifyType::StatusMessage);
}

void Scenario3::OnSaveAsync(Platform::Object^ sender, RoutedEventArgs^ e)
{
#if 0
	// We don't want to save an empty file
	if (inkCanvas->InkPresenter->StrokeContainer->GetStrokes()->Size > 0)
	{
		auto savePicker = ref new Windows::Storage::Pickers::FileSavePicker();
		savePicker->SuggestedStartLocation = Windows::Storage::Pickers::PickerLocationId::PicturesLibrary;
		savePicker->FileTypeChoices.Add("Gif with embedded ISF", new System.Collections.Generic.List<Platform::String^> { ".gif" });

		Windows.Storage.StorageFile file = await savePicker->PickSaveFileAsync();
		if (null != file)
		{
			try
			{
				using (IRandomAccessStream stream = await file.OpenAsync(FileAccessMode.ReadWrite))
				{
					await inkCanvas.InkPresenter.StrokeContainer.SaveAsync(stream);
				}
				rootPage->NotifyUser(inkCanvas.InkPresenter.StrokeContainer.GetStrokes().Count + " stroke(s) saved!", NotifyType.StatusMessage);
			}
			catch (Exception ex)
			{
				rootPage->NotifyUser(ex.Message, NotifyType.ErrorMessage);
			}
		}
	}
	else
	{
		rootPage->NotifyUser("There is no ink to save.", NotifyType::ErrorMessage);
	}
#endif
}

void Scenario3::OnLoadAsync(Platform::Object^ sender, RoutedEventArgs^ e)
{
#if 0
	var openPicker = new Windows.Storage.Pickers.FileOpenPicker();
	openPicker.SuggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.PicturesLibrary;
	openPicker.FileTypeFilter.Add(".gif");
	openPicker.FileTypeFilter.Add(".isf");
	Windows.Storage.StorageFile file = await openPicker.PickSingleFileAsync();
	if (null != file)
	{
		using (var stream = await file.OpenSequentialReadAsync())
		{
			try
			{
				await inkCanvas.InkPresenter.StrokeContainer.LoadAsync(stream);
			}
			catch (Exception ex)
			{
				rootPage->NotifyUser(ex.Message, NotifyType.ErrorMessage);
			}
		}

		rootPage->NotifyUser(inkCanvas.InkPresenter.StrokeContainer.GetStrokes().Count + " stroke(s) loaded!", NotifyType.StatusMessage);
	}
#endif
}

void Scenario3::TouchInkingCheckBox_Checked(Platform::Object^ sender, RoutedEventArgs^ e)
{
	auto types = inkCanvas->InkPresenter->InputDeviceTypes | CoreInputDeviceTypes::Touch;
	inkCanvas->InkPresenter->InputDeviceTypes = types;
	rootPage->NotifyUser("Enable Touch Inking", NotifyType::StatusMessage);
}

void Scenario3::TouchInkingCheckBox_Unchecked(Platform::Object^ sender, RoutedEventArgs^ e)
{
	auto types = inkCanvas->InkPresenter->InputDeviceTypes & ~CoreInputDeviceTypes::Touch;
	inkCanvas->InkPresenter->InputDeviceTypes = types;
	rootPage->NotifyUser("Disable Touch Inking", NotifyType::StatusMessage);
}

void Scenario3::ErasingModeCheckBox_Checked(Platform::Object^ sender, RoutedEventArgs^ e)
{
	inkCanvas->InkPresenter->InputProcessingConfiguration->Mode = InkInputProcessingMode::Erasing;
	rootPage->NotifyUser("Enable Erasing Mode", NotifyType::StatusMessage);
}

void Scenario3::ErasingModeCheckBox_Unchecked(Platform::Object^ sender, RoutedEventArgs^ e)
{
	inkCanvas->InkPresenter->InputProcessingConfiguration->Mode = InkInputProcessingMode::Inking;
	rootPage->NotifyUser("Disable Erasing Mode", NotifyType::StatusMessage);
}
