//
//  SceneDelegate.swift
//  CartoTypeSwiftDemo
//
//  Copyright © 2026 CartoType Ltd. All rights reserved.
//

import Foundation
import UIKit
import CartoType

class SceneDelegate: UIResponder, UIWindowSceneDelegate
    {

    var window: UIWindow?
    var m_framework: CartoTypeFramework!
    var m_view_controller: ViewController!

    func scene(_ scene: UIScene, willConnectTo session: UISceneSession, options connectionOptions: UIScene.ConnectionOptions)
        {
        // Ensure we have a valid UIWindowScene
        guard let windowScene = (scene as? UIWindowScene) else
            {
            return
            }
        
        // 1. Set up the Window
        let bounds = windowScene.coordinateSpace.bounds
        let window = UIWindow(windowScene: windowScene)
        self.window = window
        
        // 2. Set up CartoType Framework
        let scale = windowScene.screen.scale
        let width = bounds.width * scale
        let height = bounds.height * scale
        let param = CartoTypeFrameworkParam()
            
        param.mapFileName = "santa-cruz"

        // Uncomment the following section (and change the map file name) to use a file uploaded to the app's sandbox using the Mac finder.
        /*
        let urls = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask)
        let filename = "britain_and_ireland_tech.ctm1"
        let file_url = urls[0].appendingPathComponent(filename)
        param.mapFileName = file_url.path
        */
        
        param.styleSheetFileName = "standard"
        param.fontFileName = "DejaVuSans"
        param.viewWidth = Int32(width)
        param.viewHeight = Int32(height)
        m_framework = CartoTypeFramework(param: param)!
        m_framework.license("mylicensekey")
        m_framework.loadFont("DejaVuSerif")
        m_framework.loadFont("DejaVuSans-Bold")
        m_framework.loadFont("DejaVuSerif-Italic")
        m_framework.setViewLimitsToMinScale(1000, maxScale: 0, panArea: nil)
            
        // 3. Create and assign the View Controller
        m_view_controller = ViewController(aFrameWork: m_framework, aBounds: bounds)
        self.window?.rootViewController = m_view_controller
        self.window?.backgroundColor = UIColor.white
        self.window?.makeKeyAndVisible()
        
        loadSettings()
        }

    func sceneDidEnterBackground(_ scene: UIScene)
        {
        saveSettings()
        }

    func sceneWillEnterForeground(_ scene: UIScene)
        {
        loadSettings()
        }
    
    // MARK: - Settings Logic (Moved from AppDelegate)
    
    func loadSettings()
        {
        let defaults = UserDefaults.standard
        if let view_state_string = defaults.string(forKey: "viewState")
            {
            let view_state = CartoTypeViewState()
            view_state.read(fromXml: view_state_string)
            
            let current_view_state = m_framework.viewState!
            view_state.widthInPixels = current_view_state.widthInPixels
            view_state.heightInPixels = current_view_state.heightInPixels
            
            m_framework.viewState = view_state
            }
        
        if let route_string = defaults.string(forKey: "route"), !route_string.isEmpty
            {
            m_framework.readRoute(fromXml: route_string)
            m_view_controller.updateRouteStatus()
            }
        
        if let saved_map_objects = defaults.data(forKey: "savedMapObjects")
            {
            m_framework.readMap(0, data: saved_map_objects)
            }
        }
    
    func saveSettings()
        {
        let view_state = m_framework.viewState
        let view_state_string = view_state?.toXml()
        let defaults = UserDefaults.standard
        defaults.set(view_state_string, forKey: "viewState")
        
        var route_string = ""
        if m_framework.route != nil
            {
            route_string = m_framework.writeRoute(asXmlString: m_framework.route, fileType: CtFileType.ctroute)
            }
        defaults.set(route_string, forKey: "route")
        
        let find_param = CartoTypeFindParam()!
        find_param.layers = "pushpin"
        let saved_map_objects = m_framework.saveMap(0, param: find_param)!
        if saved_map_objects.endIndex > 1
            {
            defaults.set(saved_map_objects, forKey: "savedMapObjects")
            }
        }
    }
