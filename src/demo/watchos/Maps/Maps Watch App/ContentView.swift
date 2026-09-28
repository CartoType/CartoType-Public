/*
ContentView.swift

Part of the CartoType demonstration app for Apple Watch.
Copyright (C) 2024 CartoType Ltd.
See www.cartotype.com for more information.
*/

import SwiftUI
import CartoType

struct ContentView: View
    {
    var body: some View
        {
        Image(uiImage: m_map_image)
            .transformEffect(m_image_transform)
            .focusable(true)
            .digitalCrownRotation($m_crown_value, onChange: onCrownChange, onIdle: onCrownIdle)
            .onChange(of: m_crown_value, initial: false, onCrownRotation)
            .gesture(DragGesture().onChanged(onDragChanged).onEnded(onDragEnded))
        }
    
    func onCrownChange(aEvent: DigitalCrownEvent)
        {
        m_crown_is_idle = false
        }
    
    func onCrownIdle()
        {
        m_crown_is_idle = true
        m_framework.zoom(m_zoom)
        m_zoom = 1.0
        m_image_transform = m_initial_image_transform
        m_map_image = m_framework.mapBitmap()
        }
    
    func zoom(aZoomFactor: CGFloat)
        {
        let cx = m_width / m_scale
        let cy = m_height / m_scale
        
        m_image_transform = m_image_transform.translatedBy(x: cx, y: cy)
        m_image_transform = m_image_transform.scaledBy(x: aZoomFactor, y: aZoomFactor)
        m_image_transform = m_image_transform.translatedBy(x: -cx, y: -cy)
        }
    
    func onCrownRotation()
        {
        let zoom_factor = 1.02
        
        if (m_crown_value > m_prev_crown_value)
            {
            m_zoom *= zoom_factor
            zoom(aZoomFactor: zoom_factor)
            }
        else if (m_crown_value < m_prev_crown_value)
            {
            m_zoom /= zoom_factor
            zoom(aZoomFactor: 1.0 / zoom_factor)
            }
        m_prev_crown_value = m_crown_value
        }
    
    func onDragChanged(value: DragGesture.Value)
        {
        m_image_transform.tx = m_initial_image_transform.tx + value.translation.width
        m_image_transform.ty = m_initial_image_transform.ty + value.translation.height
        }

    func onDragEnded(value: DragGesture.Value)
        {
        let dx = m_image_transform.tx - m_initial_image_transform.tx;
        let dy = m_image_transform.ty - m_initial_image_transform.ty;
        m_framework.panX(Int32(-dx * m_scale), andY: Int32(-dy * m_scale))
        m_image_transform = m_initial_image_transform
        m_map_image = m_framework.mapBitmap()
        }

    init()
        {
        // Create the CartoType framework
        let bounds = WKInterfaceDevice.current().screenBounds
        m_scale = WKInterfaceDevice.current().screenScale
        m_width = bounds.width * m_scale
        m_height = bounds.height * m_scale
        let param = CartoTypeFrameworkParam()!
        param.mapFileName = "santa-cruz"
        param.styleSheetFileName = "standard"
        param.fontFileName = "DejaVuSans"
        param.viewWidth = Int32(m_width)
        param.viewHeight = Int32(m_height)
        m_framework = CartoTypeFramework.init(param: param)!
        m_framework.license("mylicensekey")
            
        m_map_image = m_framework.mapBitmap()
        let inverse_scale = 1.0 / m_scale
        m_initial_image_transform = CGAffineTransform(inverse_scale,0,0,inverse_scale,bounds.width * inverse_scale,bounds.height * inverse_scale)
        m_image_transform = m_initial_image_transform
        }
    
    var m_framework : CartoTypeFramework!
    var m_initial_image_transform: CGAffineTransform
    var m_scale: CGFloat = 1.0
    var m_width = 0.0
    var m_height = 0.0
    @State var m_map_image : UIImage
    @State var m_image_transform: CGAffineTransform
    @State var m_crown_value: CGFloat = 0.0
    @State var m_prev_crown_value: CGFloat = 0.0
    @State var m_crown_is_idle = true
    @State var m_zoom: CGFloat = 1.0
    }

#Preview
    {
    ContentView()
    }
