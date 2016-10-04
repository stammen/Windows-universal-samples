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
#include "Scenario2.xaml.h"
#include <WindowsNumerics.h>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Shapes;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Input::Inking;
using namespace Windows::UI::Core;

InkDrawingAttributes^ CalligraphicPen::CreateInkDrawingAttributesCore(Windows::UI::Xaml::Media::Brush^ brush, double strokeWidth)
{

	InkDrawingAttributes^ inkDrawingAttributes = ref new InkDrawingAttributes();
	inkDrawingAttributes->PenTip = PenTipShape::Circle;
	inkDrawingAttributes->IgnorePressure = false;
	SolidColorBrush^ solidColorBrush = (SolidColorBrush^)brush;

	if (solidColorBrush != nullptr)
	{
		inkDrawingAttributes->Color = solidColorBrush->Color;
	}

	inkDrawingAttributes->Size = Size((float)strokeWidth, 2.0f * (float)strokeWidth);
	inkDrawingAttributes->PenTipTransform = make_float3x2_rotation((float)((M_PI * 45.0)/180.0), float2::zero());

	return inkDrawingAttributes;
}

Scenario2::

Scenario2::Scenario2() : rootPage(MainPage::Current)
{
    InitializeComponent();
	CalligraphyPen = (Windows::UI::Xaml::Controls::Symbol)0xEDFB;
	LassoSelect = (Windows::UI::Xaml::Controls::Symbol)0xEF20;
	TouchWriting = (Windows::UI::Xaml::Controls::Symbol)0xED5F;

	// Initialize the InkCanvas
	inkCanvas->InkPresenter->InputDeviceTypes = CoreInputDeviceTypes::Mouse | CoreInputDeviceTypes::Pen;

	// Handlers to clear the selection when inking or erasing is detected
	inkCanvas->InkPresenter->StrokeInput->StrokeStarted += ref new TypedEventHandler<InkStrokeInput^, PointerEventArgs^>(this, &Scenario2::StrokeInput_StrokeStarted);
	inkCanvas->InkPresenter->StrokeInput->StrokeEnded += ref new TypedEventHandler<InkStrokeInput^, PointerEventArgs^>(this, &Scenario2::InkPresenter_StrokesErased);
}

void Scenario2::OnSizeChanged(Platform::Object^ sender, SizeChangedEventArgs^ e)
{
	HelperFunctions::UpdateCanvasSize(RootGrid, outputGrid, inkCanvas);
}

void Scenario2::StrokeInput_StrokeStarted(InkStrokeInput^ sender, PointerEventArgs^ args)
{
	ClearSelection();

}

void Scenario2::InkPresenter_StrokesErased(InkStrokeInput^ sender, PointerEventArgs^ args)
{

}

void Scenario2::Toggle_Custom(Platform::Object^ sender, RoutedEventArgs^ e)
{

}

void Scenario2::UnprocessedInput_PointerPressed(InkUnprocessedInput^ sender, PointerEventArgs^  args)
{

}

void Scenario2::UnprocessedInput_PointerMoved(InkUnprocessedInput^ sender, PointerEventArgs^  args)
{

}

void Scenario2::UnprocessedInput_PointerReleased(InkUnprocessedInput^ sender, PointerEventArgs^ args)
{

}


void Scenario2::DrawBoundingRect()
{

}


void Scenario2::ToolButton_Lasso(Platform::Object^ sender, RoutedEventArgs^ e)
{

}

void Scenario2::ClearDrawnBoundingRect()
{
	if (selectionCanvas->Children->Size > 0)
	{
		selectionCanvas->Children->Clear();
		boundingRect = Rect::Empty;
	}
}

void Scenario2::OnCopy(Platform::Object^ sender, RoutedEventArgs^ e)
{
	inkCanvas->InkPresenter->StrokeContainer->CopySelectedToClipboard();
}

void Scenario2::OnCut(Platform::Object^ sender, RoutedEventArgs^ e)
{
	inkCanvas->InkPresenter->StrokeContainer->CopySelectedToClipboard();
	inkCanvas->InkPresenter->StrokeContainer->DeleteSelected();
	ClearDrawnBoundingRect();
}

void Scenario2::OnPaste(Platform::Object^ sender, RoutedEventArgs^ e)
{
	if (inkCanvas->InkPresenter->StrokeContainer->CanPasteFromClipboard())
	{
		inkCanvas->InkPresenter->StrokeContainer->PasteFromClipboard(Point((scrollViewer->HorizontalOffset + 10) / scrollViewer->ZoomFactor, (scrollViewer->VerticalOffset + 10) / scrollViewer->ZoomFactor));
	}
	else
	{
		rootPage->NotifyUser("Cannot paste from clipboard.", NotifyType::ErrorMessage);
	}
}

void Scenario2::ClearSelection()
{
	auto strokes = inkCanvas->InkPresenter->StrokeContainer->GetStrokes();
	for(auto stroke : strokes)
	{
		stroke->Selected = false;
	}
	ClearDrawnBoundingRect();
}

void Scenario2::CurrentToolChanged(Windows::UI::Xaml::Controls::InkToolbar^ sender, Platform::Object^ args)
{
	bool enabled = sender->ActiveTool->Equals(toolButtonLasso);

	ButtonCut->IsEnabled = enabled;
	ButtonCopy->IsEnabled = enabled;
	ButtonPaste->IsEnabled = enabled;
}

