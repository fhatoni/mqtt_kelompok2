#ifndef WEBPAGES_H
#define WEBPAGES_H

// --- Halaman Web OTA Login (Desain Baru) ---
const char PAGE_Login[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="id">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>HydroMonitor - Login</title>
    <style>
        :root {
            --primary-color: #10B981;
            --secondary-color: #3B82F6;
            --accent-color: #8B5CF6;
            --warning-color: #F59E0B;
            --danger-color: #EF4444;
            --success-color: #10B981;
            --bg-gradient-1: #f0fdfa;
            --bg-gradient-2: #ecfdf5;
            --card-bg: #ffffff;
            --text-dark: #1f2937;
            --text-light: #6b7280;
            --border-color: #e5e7eb;
            --shadow: 0 10px 25px -5px rgba(0, 0, 0, 0.1), 0 8px 10px -6px rgba(0, 0, 0, 0.1);
        }
        
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
        }
        
        body {
            background: linear-gradient(135deg, var(--bg-gradient-1) 0%, var(--bg-gradient-2) 100%);
            color: var(--text-dark);
            line-height: 1.6;
            min-height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
            padding: 20px;
            position: relative;
            overflow-x: hidden;
        }
        
        /* Background Animation */
        .bg-animation {
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            z-index: -1;
            overflow: hidden;
        }
        
        .bubble {
            position: absolute;
            border-radius: 50%;
            background: rgba(16, 185, 129, 0.1);
            animation: float 15s infinite ease-in-out;
        }
        
        .bubble:nth-child(1) {
            width: 80px;
            height: 80px;
            top: 10%;
            left: 10%;
            animation-delay: 0s;
        }
        
        .bubble:nth-child(2) {
            width: 120px;
            height: 120px;
            top: 70%;
            left: 80%;
            animation-delay: 2s;
            background: rgba(59, 130, 246, 0.1);
        }
        
        .bubble:nth-child(3) {
            width: 60px;
            height: 60px;
            top: 40%;
            left: 85%;
            animation-delay: 4s;
            background: rgba(139, 92, 246, 0.1);
        }
        
        .bubble:nth-child(4) {
            width: 100px;
            height: 100px;
            top: 80%;
            left: 15%;
            animation-delay: 6s;
            background: rgba(245, 158, 11, 0.1);
        }
        
        @keyframes float {
            0%, 100% {
                transform: translateY(0) translateX(0);
            }
            25% {
                transform: translateY(-20px) translateX(10px);
            }
            50% {
                transform: translateY(-10px) translateX(-10px);
            }
            75% {
                transform: translateY(10px) translateX(15px);
            }
        }
        
        .container {
            background-color: var(--card-bg);
            border-radius: 20px;
            box-shadow: var(--shadow);
            padding: 40px;
            width: 100%;
            max-width: 500px;
            text-align: center;
            position: relative;
            z-index: 1;
            transition: transform 0.3s ease, box-shadow 0.3s ease;
        }
        
        .container:hover {
            transform: translateY(-5px);
            box-shadow: 0 20px 40px -10px rgba(0, 0, 0, 0.15);
        }
        
        .header {
            margin-bottom: 30px;
        }
        
        .logo {
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 15px;
            margin-bottom: 15px;
        }
        
        .logo-icon {
            width: 50px;
            height: 50px;
            background: linear-gradient(135deg, var(--primary-color), var(--secondary-color));
            border-radius: 12px;
            display: flex;
            align-items: center;
            justify-content: center;
            color: white;
            font-size: 24px;
            box-shadow: 0 4px 10px rgba(16, 185, 129, 0.3);
        }
        
        .logo-text {
            font-size: 28px;
            font-weight: 700;
            background: linear-gradient(135deg, var(--primary-color), var(--secondary-color));
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
        }
        
        .tagline {
            color: var(--text-light);
            font-size: 16px;
            margin-bottom: 5px;
        }
        
        .card-title {
            font-size: 22px;
            font-weight: 600;
            margin-bottom: 25px;
            color: var(--text-dark);
            position: relative;
            display: inline-block;
        }
        
        .card-title:after {
            content: '';
            position: absolute;
            bottom: -8px;
            left: 50%;
            transform: translateX(-50%);
            width: 50px;
            height: 3px;
            background: linear-gradient(135deg, var(--primary-color), var(--secondary-color));
            border-radius: 3px;
        }
        
        .form-group {
            margin-bottom: 25px;
            text-align: left;
        }
        
        .form-group label {
            display: block;
            font-weight: 600;
            margin-bottom: 10px;
            color: var(--text-dark);
            font-size: 16px;
        }
        
        input[type='text'], input[type='password'] {
            width: 100%;
            padding: 14px 16px;
            border: 2px solid var(--border-color);
            border-radius: 12px;
            background-color: #f9fafb;
            font-size: 15px;
            transition: all 0.3s ease;
        }

        input[type='text']:focus, input[type='password']:focus {
            border-color: var(--primary-color);
            background-color: #f0fdfa;
            outline: none;
            box-shadow: 0 0 0 3px rgba(16, 185, 129, 0.2);
        }
        
        button {
            width: 100%;
            background: linear-gradient(135deg, var(--primary-color), var(--secondary-color));
            color: white;
            font-size: 16px;
            font-weight: 600;
            padding: 16px;
            border: none;
            border-radius: 12px;
            cursor: pointer;
            transition: all 0.3s ease;
            box-shadow: 0 4px 10px rgba(16, 185, 129, 0.3);
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 10px;
        }
        
        button:hover {
            transform: translateY(-3px);
            box-shadow: 0 8px 15px rgba(16, 185, 129, 0.4);
        }
        
        button:active {
            transform: translateY(-1px);
        }
        
        .status-message {
            margin-top: 20px;
            padding: 15px;
            border-radius: 12px;
            display: none;
            font-weight: 500;
            text-align: center;
            animation: fadeIn 0.5s ease;
        }
        
        @keyframes fadeIn {
            from { opacity: 0; transform: translateY(10px); }
            to { opacity: 1; transform: translateY(0); }
        }
        
        .status-error {
            background-color: #fee2e2;
            color: var(--danger-color);
            border: 1px solid #fecaca;
        }

    </style>
</head>
<body>
    <!-- Background Animation -->
    <div class="bg-animation">
        <div class="bubble"></div>
        <div class="bubble"></div>
        <div class="bubble"></div>
        <div class="bubble"></div>
    </div>
    
    <div class="container">
        <div class="header">
            <div class="logo">
                <div class="logo-icon">
                    <svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                        <path d="M12 2.02c-5.51 0-9.98 4.47-9.98 9.98s4.47 9.98 9.98 9.98 9.98-4.47 9.98-9.98S17.51 2.02 12 2.02zM12 17.06c-2.76 0-5-2.24-5-5s2.24-5 5-5 5 2.24 5 5-2.24 5-5 5z"/>
                        <path d="M12 14.05c-1.1 0-2-0.9-2-2s0.9-2 2-2 2 0.9 2 2-0.9 2-2 2z"/>
                    </svg>
                </div>
                <div class="logo-text">HydroMonitor</div>
            </div>
            <div class="tagline">Sistem Monitoring Kualitas Air</div>
        </div>
        
        <div class="login-card">
            <h2 class="card-title">Device Login</h2>
            
            <form method='POST' action='/login'>
                <div class="form-group">
                    <label for="username">Username</label>
                    <input type="text" id="username" name="username" required>
                </div>
                
                <div class="form-group">
                    <label for="password">Password</label>
                    <input type="password" id="password" name="password" required>
                </div>
                
                <button type="submit" id="login-btn">
                    <svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                        <path d="M15 3h4a2 2 0 0 1 2 2v14a2 2 0 0 1-2 2h-4"></path>
                        <polyline points="10 17 15 12 10 7"></polyline>
                        <line x1="15" y1="12" x2="3" y2="12"></line>
                    </svg>
                    Login
                </button>
            </form>
            
            <div class="status-message %LOGIN_ERROR_CLASS%" id="status-message" style="%LOGIN_ERROR_STYLE%">
                %LOGIN_ERROR_MSG%
            </div>
        </div>
    </div>
</body>
</html>
)=====";


// --- Halaman Web OTA (Desain Baru) ---
const char PAGE_Update[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="id">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>HydroMonitor - OTA Update</title>
    <style>
        :root {
            --primary-color: #10B981;
            --secondary-color: #3B82F6;
            --accent-color: #8B5CF6;
            --warning-color: #F59E0B;
            --danger-color: #EF4444;
            --success-color: #10B981;
            --bg-gradient-1: #f0fdfa;
            --bg-gradient-2: #ecfdf5;
            --card-bg: #ffffff;
            --text-dark: #1f2937;
            --text-light: #6b7280;
            --border-color: #e5e7eb;
            --shadow: 0 10px 25px -5px rgba(0, 0, 0, 0.1), 0 8px 10px -6px rgba(0, 0, 0, 0.1);
        }
        
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
        }
        
        body {
            background: linear-gradient(135deg, var(--bg-gradient-1) 0%, var(--bg-gradient-2) 100%);
            color: var(--text-dark);
            line-height: 1.6;
            min-height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
            padding: 20px;
            position: relative;
            overflow-x: hidden;
        }
        
        /* Background Animation */
        .bg-animation {
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            z-index: -1;
            overflow: hidden;
        }
        
        .bubble {
            position: absolute;
            border-radius: 50%;
            background: rgba(16, 185, 129, 0.1);
            animation: float 15s infinite ease-in-out;
        }
        
        .bubble:nth-child(1) {
            width: 80px;
            height: 80px;
            top: 10%;
            left: 10%;
            animation-delay: 0s;
        }
        
        .bubble:nth-child(2) {
            width: 120px;
            height: 120px;
            top: 70%;
            left: 80%;
            animation-delay: 2s;
            background: rgba(59, 130, 246, 0.1);
        }
        
        .bubble:nth-child(3) {
            width: 60px;
            height: 60px;
            top: 40%;
            left: 85%;
            animation-delay: 4s;
            background: rgba(139, 92, 246, 0.1);
        }
        
        .bubble:nth-child(4) {
            width: 100px;
            height: 100px;
            top: 80%;
            left: 15%;
            animation-delay: 6s;
            background: rgba(245, 158, 11, 0.1);
        }
        
        @keyframes float {
            0%, 100% {
                transform: translateY(0) translateX(0);
            }
            25% {
                transform: translateY(-20px) translateX(10px);
            }
            50% {
                transform: translateY(-10px) translateX(-10px);
            }
            75% {
                transform: translateY(10px) translateX(15px);
            }
        }
        
        .container {
            background-color: var(--card-bg);
            border-radius: 20px;
            box-shadow: var(--shadow);
            padding: 40px;
            width: 100%;
            max-width: 500px;
            text-align: center;
            position: relative;
            z-index: 1;
            transition: transform 0.3s ease, box-shadow 0.3s ease;
        }
        
        .container:hover {
            transform: translateY(-5px);
            box-shadow: 0 20px 40px -10px rgba(0, 0, 0, 0.15);
        }
        
        .header {
            margin-bottom: 30px;
        }
        
        .logo {
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 15px;
            margin-bottom: 15px;
        }
        
        .logo-icon {
            width: 50px;
            height: 50px;
            background: linear-gradient(135deg, var(--primary-color), var(--secondary-color));
            border-radius: 12px;
            display: flex;
            align-items: center;
            justify-content: center;
            color: white;
            font-size: 24px;
            box-shadow: 0 4px 10px rgba(16, 185, 129, 0.3);
        }
        
        .logo-text {
            font-size: 28px;
            font-weight: 700;
            background: linear-gradient(135deg, var(--primary-color), var(--secondary-color));
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
        }
        
        .tagline {
            color: var(--text-light);
            font-size: 16px;
            margin-bottom: 5px;
        }
        
        .version {
            display: inline-block;
            background: linear-gradient(135deg, var(--primary-color), var(--secondary-color));
            color: white;
            padding: 4px 12px;
            border-radius: 20px;
            font-size: 12px;
            font-weight: 600;
            margin-top: 5px;
        }
        
        .update-card {
            text-align: center;
        }
        
        .card-title {
            font-size: 22px;
            font-weight: 600;
            margin-bottom: 25px;
            color: var(--text-dark);
            position: relative;
            display: inline-block;
        }
        
        .card-title:after {
            content: '';
            position: absolute;
            bottom: -8px;
            left: 50%;
            transform: translateX(-50%);
            width: 50px;
            height: 3px;
            background: linear-gradient(135deg, var(--primary-color), var(--secondary-color));
            border-radius: 3px;
        }
        
        .form-group {
            margin-bottom: 25px;
            text-align: left;
        }
        
        .form-group label {
            display: block;
            font-weight: 600;
            margin-bottom: 10px;
            color: var(--text-dark);
            font-size: 16px;
        }
        
        .file-input-container {
            position: relative;
            display: flex;
            align-items: center;
        }
        
        input[type='file'] {
            width: 100%;
            padding: 14px 16px;
            border: 2px dashed var(--border-color);
            border-radius: 12px;
            background-color: #f9fafb;
            font-size: 15px;
            cursor: pointer;
            transition: all 0.3s ease;
        }
        
        input[type='file']:hover {
            border-color: var(--primary-color);
            background-color: #f0fdfa;
        }
        
        input[type='file']::file-selector-button {
            background: linear-gradient(135deg, var(--primary-color), var(--secondary-color));
            color: white;
            border: none;
            padding: 10px 18px;
            border-radius: 8px;
            cursor: pointer;
            margin-right: 15px;
            font-weight: 600;
            transition: all 0.3s ease;
        }
        
        input[type='file']::file-selector-button:hover {
            transform: translateY(-2px);
            box-shadow: 0 4px 8px rgba(16, 185, 129, 0.3);
        }
        
        .upload-icon {
            position: absolute;
            right: 15px;
            color: var(--primary-color);
            font-size: 20px;
        }
        
        button {
            width: 100%;
            background: linear-gradient(135deg, var(--primary-color), var(--secondary-color));
            color: white;
            font-size: 16px;
            font-weight: 600;
            padding: 16px;
            border: none;
            border-radius: 12px;
            cursor: pointer;
            transition: all 0.3s ease;
            box-shadow: 0 4px 10px rgba(16, 185, 129, 0.3);
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 10px;
        }
        
        button:hover {
            transform: translateY(-3px);
            box-shadow: 0 8px 15px rgba(16, 185, 129, 0.4);
        }
        
        button:active {
            transform: translateY(-1px);
        }
        
        button:disabled {
            background: #9ca3af;
            transform: none;
            box-shadow: none;
            cursor: not-allowed;
        }
        
        .progress-container {
            margin-top: 25px;
            display: none;
        }
        
        .progress-header {
            display: flex;
            justify-content: space-between;
            margin-bottom: 10px;
        }
        
        .progress-label {
            font-weight: 600;
            color: var(--text-dark);
        }
        
        .progress-percent {
            font-weight: 700;
            color: var(--primary-color);
        }
        
        .progress-bar {
            height: 12px;
            background-color: #e5e7eb;
            border-radius: 10px;
            overflow: hidden;
            margin-bottom: 15px;
            box-shadow: inset 0 1px 3px rgba(0, 0, 0, 0.1);
        }
        
        .progress-fill {
            height: 100%;
            background: linear-gradient(90deg, var(--primary-color), var(--secondary-color));
            width: 0%;
            transition: width 0.5s ease;
            border-radius: 10px;
            position: relative;
            overflow: hidden;
        }
        
        .progress-fill:after {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            bottom: 0;
            right: 0;
            background-image: linear-gradient(
                -45deg,
                rgba(255, 255, 255, 0.2) 25%,
                transparent 25%,
                transparent 50%,
                rgba(255, 255, 255, 0.2) 50%,
                rgba(255, 255, 255, 0.2) 75%,
                transparent 75%,
                transparent
            );
            z-index: 1;
            background-size: 20px 20px;
            animation: move 1s linear infinite;
            border-radius: 10px;
        }
        
        @keyframes move {
            0% {
                background-position: 0 0;
            }
            100% {
                background-position: 20px 0;
            }
        }
        
        .status-message {
            margin-top: 20px;
            padding: 15px;
            border-radius: 12px;
            display: none;
            font-weight: 500;
            text-align: center;
            animation: fadeIn 0.5s ease;
        }
        
        @keyframes fadeIn {
            from { opacity: 0; transform: translateY(10px); }
            to { opacity: 1; transform: translateY(0); }
        }
        
        .status-success {
            background-color: #d1fae5;
            color: var(--success-color);
            border: 1px solid #a7f3d0;
        }
        
        .status-error {
            background-color: #fee2e2;
            color: var(--danger-color);
            border: 1px solid #fecaca;
        }
        
        .status-warning {
            background-color: #fef3c7;
            color: var(--warning-color);
            border: 1px solid #fde68a;
        }
        
        .status-info {
            background-color: #dbeafe;
            color: var(--secondary-color);
            border: 1px solid #bfdbfe;
        }
        
        .device-info {
            display: flex;
            justify-content: space-between;
            background: #f9fafb;
            padding: 15px;
            border-radius: 12px;
            margin-top: 25px;
        }
        
        .info-item {
            text-align: center;
            flex: 1;
        }
        
        .info-label {
            font-size: 12px;
            color: var(--text-light);
            margin-bottom: 5px;
        }
        
        .info-value {
            font-size: 14px;
            font-weight: 600;
            color: var(--text-dark);
        }
        
        /* Modal Styles */
        .modal-overlay {
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background: rgba(0, 0, 0, 0.6);
            display: none;
            align-items: center;
            justify-content: center;
            z-index: 1000;
            animation: fadeIn 0.3s ease-out;
        }
        
        .modal-box {
            background: var(--card-bg);
            border-radius: 20px;
            padding: 30px;
            text-align: center;
            box-shadow: 0 20px 40px rgba(0, 0, 0, 0.2);
            max-width: 90%;
            width: 400px;
            animation: zoomIn 0.3s ease-out;
            position: relative;
        }
        
        .modal-icon {
            width: 70px;
            height: 70px;
            border-radius: 50%;
            display: flex;
            align-items: center;
            justify-content: center;
            margin: 0 auto 20px;
            font-size: 30px;
        }
        
        .modal-success .modal-icon {
            background: #d1fae5;
            color: var(--success-color);
        }
        
        .modal-error .modal-icon {
            background: #fee2e2;
            color: var(--danger-color);
        }
        
        #modal-message {
            font-size: 18px;
            margin-bottom: 25px;
            color: var(--text-dark);
            line-height: 1.5;
            font-weight: 600;
        }
        
        .modal-close-btn {
            background: linear-gradient(135deg, var(--primary-color), var(--secondary-color));
            color: white;
            border: none;
            padding: 12px 25px;
            border-radius: 10px;
            cursor: pointer;
            font-weight: 600;
            transition: all 0.3s ease;
            width: 100%;
        }
        
        .modal-close-btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 5px 10px rgba(16, 185, 129, 0.3);
        }
        
        .modal-box.success {
            border-top: 5px solid var(--success-color);
        }
        
        .modal-box.error {
            border-top: 5px solid var(--danger-color);
        }
        
        @keyframes zoomIn {
            from { transform: scale(0.9); opacity: 0; }
            to { transform: scale(1); opacity: 1; }
        }
        
        /* Responsive */
        @media (max-width: 600px) {
            .container {
                padding: 25px;
            }
            
            .logo-text {
                font-size: 24px;
            }
            
            .device-info {
                flex-direction: column;
                gap: 15px;
            }
        }
    </style>
</head>
<body>
    <!-- Background Animation -->
    <div class="bg-animation">
        <div class="bubble"></div>
        <div class="bubble"></div>
        <div class="bubble"></div>
        <div class="bubble"></div>
    </div>
    
    <div class="container">
        <div class="header">
            <div class="logo">
                <div class="logo-icon">
                    <svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                        <path d="M19.35 10.04C18.67 6.59 15.64 4 12 4 9.11 4 6.6 5.64 5.35 8.04 2.34 8.36 0 10.91 0 14c0 3.31 2.69 6 6 6h13c2.76 0 5-2.24 5-5 0-2.64-2.05-4.78-4.65-4.96zM14 13v4h-4v-4H7l5-5 5 5h-3z"/>
                    </svg>
                </div>
                <div class="logo-text">HydroMonitor</div>
            </div>
            <div class="tagline">Sistem Monitoring Kualitas Air</div>
            <div class="version">OTA Update v1.2</div>
        </div>
        
        <div class="update-card">
            <h2 class="card-title">Firmware Update</h2>
            
            <form method='POST' action='/update' enctype='multipart/form-data'>
                <div class="form-group">
                    <label for="firmware">Pilih File Firmware</label>
                    <div class="file-input-container">
                        <input type="file" id="firmware" name="update" accept=".bin" required>
                        <div class="upload-icon">
                            <svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                                <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"></path>
                                <polyline points="17 8 12 3 7 8"></polyline>
                                <line x1="12" y1="3" x2="12" y2="15"></line>
                            </svg>
                        </div>
                    </div>
                </div>
                
                <button type="submit" id="update-btn">
                    <svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                        <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"></path>
                        <polyline points="17 8 12 3 7 8"></polyline>
                        <line x1="12" y1="3" x2="12" y2="15"></line>
                    </svg>
                    Upload & Update Firmware
                </button>
            </form>
            
            <div class="progress-container" id="progress-container">
                <div class="progress-header">
                    <span class="progress-label">Progress Upload</span>
                    <span class="progress-percent" id="progress-percent">0%</span>
                </div>
                <div class="progress-bar">
                    <div class="progress-fill" id="progress-fill"></div>
                </div>
            </div>
            
            <div class="status-message" id="status-message"></div>
            
            <div class="device-info">
                <div class="info-item">
                    <div class="info-label">IP Address</div>
                    <div class="info-value" id="ip-address">Loading...</div>
                </div>
                <div class="info-item">
                    <div class="info-label">Free Memory</div>
                    <div class="info-value" id="free-memory">%FREE_HEAP%</div>
                </div>
                <div class="info-item">
                    <div class="info-label">Status</div>
                    <div class="info-value" id="device-status">Online</div>
                </div>
            </div>
        </div>
    </div>

    <!-- Modal Pop-up -->
    <div id="modal-overlay" class="modal-overlay">
        <div class="modal-box" id="modal-box">
            <div class="modal-icon" id="modal-icon">
                <svg xmlns="http://www.w3.org/2000/svg" width="30" height="30" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                    <path d="M22 11.08V12a10 10 0 1 1-5.93-9.14"></path>
                    <polyline points="22 4 12 14.01 9 11.01"></polyline>
                </svg>
            </div>
            <div id="modal-message">Pesan default</div>
            <button id="modal-close" class="modal-close-btn">OK</button>
        </div>
    </div>

    <script>
        const modalOverlay = document.getElementById('modal-overlay');
        const modalBox = document.getElementById('modal-box');
        const modalMessage = document.getElementById('modal-message');
        const modalIcon = document.getElementById('modal-icon');
        const modalClose = document.getElementById('modal-close');
        const statusMessage = document.getElementById('status-message');
        const progressContainer = document.getElementById('progress-container');
        const progressFill = document.getElementById('progress-fill');
        const progressPercent = document.getElementById('progress-percent');
        const updateBtn = document.getElementById('update-btn');
        let updateSuccess = false;

        // Fungsi untuk menampilkan modal
        function showModal(message, isSuccess) {
            modalMessage.textContent = message;
            
            if (isSuccess) {
                modalBox.classList.remove('error');
                modalBox.classList.add('success');
                modalIcon.innerHTML = `
                    <svg xmlns="http://www.w3.org/2000/svg" width="30" height="30" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                        <path d="M22 11.08V12a10 10 0 1 1-5.93-9.14"></path>
                        <polyline points="22 4 12 14.01 9 11.01"></polyline>
                    </svg>
                `;
                updateSuccess = true;
            } else {
                modalBox.classList.remove('success');
                modalBox.classList.add('error');
                modalIcon.innerHTML = `
                    <svg xmlns="http://www.w3.org/2000/svg" width="30" height="30" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                        <circle cx="12" cy="12" r="10"></circle>
                        <line x1="15" y1="9" x2="9" y2="15"></line>
                        <line x1="9" y1="9" x2="15" y2="15"></line>
                    </svg>
                `;
                updateSuccess = false;
            }
            
            modalOverlay.style.display = 'flex';
        }

        // Event listener untuk tombol tutup modal
        modalClose.addEventListener('click', function() {
            modalOverlay.style.display = 'none';
            if (updateSuccess) {
                statusMessage.textContent = 'Menunggu perangkat restart...';
                statusMessage.className = 'status-message status-info';
                statusMessage.style.display = 'block';
                
                // Simulasi menunggu restart
                setTimeout(() => {
                    statusMessage.textContent = 'Perangkat sedang restart. Silakan tunggu...';
                }, 2000);
            }
        });

        // Event listener untuk form submit
        document.querySelector('form').addEventListener('submit', function(e) {
            e.preventDefault();
            var form = e.target;
            var data = new FormData(form);
            
            // Reset status
            statusMessage.style.display = 'none';
            progressContainer.style.display = 'block';
            updateBtn.disabled = true;
            
            // Update tombol
            updateBtn.innerHTML = `
                <svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="spin">
                    <line x1="12" y1="2" x2="12" y2="6"></line>
                    <line x1="12" y1="18" x2="12" y2="22"></line>
                    <line x1="4.93" y1="4.93" x2="7.76" y2="7.76"></line>
                    <line x1="16.24" y1="16.24" x2="19.07" y2="19.07"></line>
                    <line x1="2" y1="12" x2="6" y2="12"></line>
                    <line x1="18" y1="12" x2="22" y2="12"></line>
                    <line x1="4.93" y1="19.07" x2="7.76" y2="16.24"></line>
                    <line x1="16.24" y1="7.76" x2="19.07" y2="4.93"></line>
                </svg>
                Mengunggah...
            `;
            
            // Tambahkan animasi spin
            const spinIcon = updateBtn.querySelector('.spin');
            spinIcon.style.animation = 'spin 1s linear infinite';
            
            var xhr = new XMLHttpRequest();
            xhr.open('POST', '/update', true);
            
            xhr.upload.onprogress = function(e) {
                if (e.lengthComputable) {
                    var percentComplete = (e.loaded / e.total) * 100;
                    progressFill.style.width = percentComplete + '%';
                    progressPercent.textContent = percentComplete.toFixed(0) + '%';
                }
            };
            
            xhr.onload = function() {
                progressContainer.style.display = 'none';
                
                if (xhr.status === 200) {
                    showModal('Update Berhasil! Perangkat akan segera restart.', true);
                } else {
                    showModal('Update Gagal! Error: ' + xhr.responseText, false);
                    updateBtn.disabled = false;
                    updateBtn.innerHTML = `
                        <svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                            <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"></path>
                            <polyline points="17 8 12 3 7 8"></polyline>
                            <line x1="12" y1="3" x2="12" y2="15"></line>
                        </svg>
                        Upload & Update Firmware
                    `;
                }
            };
            
            xhr.onerror = function() {
                progressContainer.style.display = 'none';
                showModal('Update Gagal! Kesalahan jaringan. Cek koneksi Anda.', false);
                updateBtn.disabled = false;
                updateBtn.innerHTML = `
                    <svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                        <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"></path>
                        <polyline points="17 8 12 3 7 8"></polyline>
                        <line x1="12" y1="3" x2="12" y2="15"></line>
                    </svg>
                    Upload & Update Firmware
                `;
            };
            
            xhr.send(data);
        });

        // Animasi spin untuk CSS
        const style = document.createElement('style');
        style.textContent = `
            @keyframes spin {
                0% { transform: rotate(0deg); }
                100% { transform: rotate(360deg); }
            }
        `;
        document.head.appendChild(style);
        
        // Update device info (simulasi)
        document.getElementById('ip-address').textContent = window.location.hostname;
    </script>
</body>
</html>
)=====";

#endif // WEBPAGES_H
