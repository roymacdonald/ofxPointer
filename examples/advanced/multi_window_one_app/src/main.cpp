//
// Copyright (c) 2019 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:	MIT
//


#include "ofApp.h"
#include "ofAppGLFWWindow.h"


int main()
{
    ofGLFWWindowSettings settings;

    settings.setSize(600, 600);
    settings.setPosition(glm::vec2(300, 0));
    settings.resizable = true;
    auto mainWindow = ofCreateWindow(settings);


    settings.setSize(300, 300);
    settings.setPosition(glm::vec2(0, 0));
    settings.resizable = false;
    auto secondWindow = ofCreateWindow(settings);
    secondWindow->setVerticalSync(false);
    secondWindow->setWindowTitle("Second Window");

    auto mainApp = std::make_shared<ofApp>();

    ofx::RegisterPointerEventForWindow(mainWindow.get(), mainApp.get());
    ofx::RegisterPointerEventForWindow(secondWindow.get(), mainApp.get());

    // Set up renderers for each window.
    ofx::PointerDebugRenderer::Settings rendererSettings;
    rendererSettings.strokeWidth = 50;
    mainApp.get()->renderers[mainWindow.get()] = ofx::PointerDebugRenderer(rendererSettings);
    rendererSettings.strokeWidth = 100;
    rendererSettings.pointColor = ofColor::red;
    mainApp.get()->renderers[secondWindow.get()] = ofx::PointerDebugRenderer(rendererSettings);

    ofAddListener(secondWindow->events().draw,
                  mainApp.get(),
                  &ofApp::drawSecondWindow);

    ofRunApp(mainWindow, mainApp);

    return ofRunMainLoop();

}
