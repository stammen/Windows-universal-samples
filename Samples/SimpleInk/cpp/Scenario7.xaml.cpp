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
#include "Scenario7.xaml.h"

#include <WindowsNumerics.h>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Input::Inking;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Foundation::Numerics;

Scenario7::Scenario7() : rootPage(MainPage::Current)
{
    InitializeComponent();

	InkCanvas->InkPresenter->InputDeviceTypes = CoreInputDeviceTypes::Mouse | CoreInputDeviceTypes::Pen;
	ruler = ref new InkPresenterRuler(InkCanvas->InkPresenter);

	// Customize Ruler
	ruler->BackgroundColor = Windows::UI::Colors::PaleTurquoise;
	ruler->ForegroundColor = Windows::UI::Colors::MidnightBlue;
	ruler->Length = 800;

	InkToolbar->IsRulerButtonCheckedChanged += ref new TypedEventHandler<Windows::UI::Xaml::Controls::InkToolbar^, Platform::Object^>(this, &Scenario7::InkToolbar_IsRulerButtonCheckedChanged);

}

void Scenario7::InkToolbar_IsRulerButtonCheckedChanged(Windows::UI::Xaml::Controls::InkToolbar^ sender, Platform::Object^ args)
{
	auto rulerButton = (InkToolbarRulerButton^)InkToolbar->GetToggleButton(InkToolbarToggle::Ruler);
	BringIntoViewButton->IsEnabled = rulerButton->IsChecked->Value;
}

void Scenario7::OnNavigatedTo(NavigationEventArgs^ e)
{
	// Make the ink canvas larger than the window, so that we can demonstrate
	// scrolling and zooming.
	InkCanvas->Width = Window::Current->Bounds.Width * 2;
	InkCanvas->Height = Window::Current->Bounds.Height * 2;
}

void Scenario7::OnBringIntoView(Platform::Object^ sender, RoutedEventArgs^ e)
{
	// Set Ruler Origin to Scrollviewer Viewport origin.
	// The purpose of this behavior is to allow the user to "grab" the
	// ruler and bring it into view no matter where the scrollviewer viewport
	// happens to be.  Note that this is accomplished by a simple translation
	// that adjusts to the zoom factor.  The additional ZoomFactor term is to
	// make ensure the scale of the InkPresenterRuler is invariant to Zoom.

	float3x2 viewportTransform = make_float3x2_scale(ScrollViewer->ZoomFactor) *
		make_float3x2_translation((float)ScrollViewer->HorizontalOffset, (float)ScrollViewer->VerticalOffset) * 
		make_float3x2_scale(1.0f / ScrollViewer->ZoomFactor);

	ruler->Transform = viewportTransform;
}