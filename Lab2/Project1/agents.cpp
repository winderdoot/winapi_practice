#include "App.h"
#include "resource.h"


void App::add_bullet()
{
	if (bullet_count >= MAX_BULLETS)
		return;
	POINT pos =
	{
		m_player.pos.x + m_player_size.x / 2 - bullet_size.x / 2,
		m_player.pos.y - bullet_size.y / 2
	};
	agent_t newBullet =
	{
		.window = CreateWindowExW(
				0,
				L"STATIC",
				nullptr,
				WS_CHILD | WS_VISIBLE | SS_CENTER,
				pos.x,
				pos.y,
				bullet_size.x,
				bullet_size.y,
				m_main,
				nullptr,
				m_instance,
				nullptr),
		.pos = pos,
	}; // Reszta nie ma znaczenia bo źle zaprojektowałem strukturę xd
	//m_bullets.push_back(agent_t())
	m_bullets[bullet_end] = newBullet;
	bullet_count++;
	bullet_end = (bullet_end + 1) % MAX_BULLETS;
	ShowWindow(newBullet.window, SW_SHOW);
}

void App::update_bullets()
{
	int aliveBuls = bullet_count;
	for (int i = bullet_beg, p = 0; p < aliveBuls; i = (i + 1) % MAX_BULLETS, p++)
	{
		m_bullets[i].pos.y -= bullet_step;
		if (m_bullets[i].pos.y < 0)
		{
			DestroyWindow(m_bullets[i].window);
			memset(&m_bullets[i], 0, sizeof(agent_t));
			bullet_count--;
			bullet_beg = (bullet_beg + 1) % MAX_BULLETS;
			//memset(m_bullets)
		}
		else
		{
			MoveWindow(m_bullets[i].window, m_bullets[i].pos.x, m_bullets[i].pos.y, bullet_size.x, bullet_size.y, true);
			InvalidateRect(m_bullets[i].window, NULL, true);
		}
	}
}

void App::enemy_reset_pos(bool resetY)
{
	// What no linear algebra does to a mf, to możnaby wszyściutko zrobić jedną macierzą rozszerzenia / zwężenia ekranu ale fuck it

	POINT curr_pos_offset =
	{
		m_enemies[0].pos.x - m_enemies[0].init_pos.x,
		m_enemies[0].pos.y - m_enemies[0].init_pos.y
	};
	LONG groupXoffset = (enemies_cols - 1) * (m_enemy_size.x / 2);
	LONG groupYoffset = (enemies_rows - 1) * (m_enemy_size.y / 2);
	m_enemies[0].pos =
	{
		m_enemy_def_pos.x + curr_pos_offset.x - groupXoffset,
		m_enemy_def_pos.y + curr_pos_offset.y - groupYoffset
	};

	for (int row = 0; row < enemies_rows; row++)
	{
		for (int col = 0; col < enemies_cols; col++)
		{
			int i = row * enemies_cols + col;
			LONG offsetX = col * m_enemy_size.x;
			LONG offsetY = row * m_enemy_size.y;
			POINT newPos { offsetX + m_enemies[0].pos.x, offsetY + m_enemies[0].pos.y };

			m_enemies[i].pos = newPos;
			m_enemies[i].init_pos =
			{
				m_enemy_def_pos.x + offsetX - groupXoffset,
				m_enemy_def_pos.y + offsetY - groupYoffset
			};
			if (IsWindowVisible(m_enemies[i].window))
				ShowWindow(m_enemies[i].window, SW_SHOW);
		}
	}
}

void App::player_reset_pos() // Też updejtuje pociski bo tak
{
	FLOAT xStretch = (FLOAT)m_player_def_pos.x / m_player.init_pos.x, yStretch = (FLOAT)m_player_def_pos.y / m_player.init_pos.y;

	for (int i = 0; i < MAX_BULLETS; i++)
	{
		POINT bulletOff = { (LONG)floor(xStretch * (m_bullets[i].pos.x - m_player.init_pos.x)), (LONG)floor(yStretch * (m_bullets[i].pos.y - m_player.init_pos.y)) };
		m_bullets[i].pos = { m_player_def_pos.x + bulletOff.x, m_player_def_pos.y + bulletOff.y };
	}


	POINT curr_pos_offset =
	{
		m_player.pos.x - m_player.init_pos.x,
		m_player.pos.y - m_player.init_pos.y,
	};
	m_player.pos = { m_player_def_pos.x + curr_pos_offset.x, m_player_def_pos.y + curr_pos_offset.y };
	m_player.init_pos = m_player_def_pos;
	MoveWindow(m_player.window, m_player.pos.x, m_player.pos.y, m_player_size.x, m_player_size.y, true);
	capture_background(m_player);
	SendMessageW(m_player.window, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)m_player.current_frame);
}

void App::enemy_move()
{
	HBITMAP prev = m_enemies[0].current_frame;
	m_enemyFrame = (m_enemyFrame + 1) % 3;

	HBITMAP curr = get_next_frame(m_enemies[0], m_enemyFrame, m_enemy_size.x, m_enemy_size.y, 4, ENEMY_BACKGROUND);
	for (auto& enem : m_enemies)
	{
		enem.current_frame = curr;
		
	}
	DeleteObject(prev);


	LONG groupoffset = (enemies_cols - 1) * (m_enemy_size.x / 2);
	LONG maxright = m_enemy_off_max + m_enemies[0].init_pos.x;
	LONG maxleft = -m_enemy_off_max + m_enemies[0].init_pos.x;
	LONG ystep = 5;

	if (m_enemy_moving_right)
	{
		LONG newX = m_enemies[0].pos.x + m_enemy_step;
		if (newX <= maxright)
		{
			for (auto& enemy : m_enemies)
				enemy.pos.x += m_enemy_step;
		}
		else
		{
			for (auto& enemy : m_enemies)
			{
				enemy.pos.x = enemy.init_pos.x + m_enemy_off_max;
				enemy.pos.y += m_enemy_ystep;
			}
			m_enemy_moving_right = false;
		}
		for (auto &enemy : m_enemies)
		{
			MoveWindow(enemy.window, enemy.pos.x, enemy.pos.y, m_enemy_size.x, m_enemy_size.y, true);
			//InvalidateRect(enemy.window, NULL, true);
			//capture_background(enemy);
			SendMessageW(enemy.window, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)enemy.current_frame);
		}
		return;
	}
	else
	{
		LONG newX = m_enemies[0].pos.x - m_enemy_step;
		if (newX >= maxleft)
		{
			for (auto& enemy : m_enemies)
				enemy.pos.x -= m_enemy_step;
		}
		else
		{
			for (auto& enemy : m_enemies)
			{
				enemy.pos.x = enemy.init_pos.x - m_enemy_off_max;
				enemy.pos.y += m_enemy_ystep;
			}
			m_enemy_moving_right = true;
		}
		for (auto& enemy : m_enemies)
		{
			MoveWindow(enemy.window, enemy.pos.x, enemy.pos.y, m_enemy_size.x, m_enemy_size.y, true);
			//InvalidateRect(enemy.window, NULL, true);
			//capture_background(enemy);
			SendMessageW(enemy.window, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)enemy.current_frame);
		}
		return;
	}
}

void App::init_enemies()
{
	HBITMAP enemyBitmap = (HBITMAP)LoadImageW(
		m_instance,      // Instance handle
		MAKEINTRESOURCE(ID_ENEMY_BMP), // Path to the .bmp file
		IMAGE_BITMAP,   // Load as a bitmap
		0, 0,           // Size (0,0 for original size)W
		LR_DEFAULTCOLOR // Load from file
	);;
	//HBITMAP prev = m_enemies[0].current_frame;


	//DeleteObject(m_player.previous_frame);
	LONG groupXoffset = (enemies_cols - 1) * (m_enemy_size.x / 2);
	LONG groupYoffset = (enemies_rows - 1) * (m_enemy_size.y / 2);

	m_enemies = std::vector<agent_t>(enemies_cols * enemies_rows);
	for (int row = 0; row < enemies_rows; row++)
	{
		for (int col = 0; col < enemies_cols; col++)
		{
			agent_t& enem = m_enemies[row * enemies_cols + col];
			enem.pos.x = m_enemy_def_pos.x + m_enemy_size.x * col - groupXoffset;
			enem.pos.y = m_enemy_def_pos.y + m_enemy_size.y * row - groupYoffset;
			enem.init_pos.x = enem.pos.x;
			enem.init_pos.y = enem.pos.y;
			enem.bitmap = enemyBitmap;
			//enem.current_frame = currframe; Fuck nie mam tego jeszcze
			enem.window = CreateWindowExW(
				0,
				L"STATIC",
				nullptr,
				WS_CHILD | WS_VISIBLE | SS_BITMAP,
				enem.pos.x,
				enem.pos.y,
				m_enemy_size.x,
				m_enemy_size.y,
				m_main,
				nullptr,
				m_instance,
				nullptr);
			//capture_background(enem);
		}
	}

	capture_background(m_enemies[0]);
	for (auto& enem : m_enemies)
		enem.background = m_enemies[0].background;

	HBITMAP currframe = get_next_frame(m_enemies[0], m_enemyFrame, m_enemy_size.x, m_enemy_size.y, 4, ENEMY_BACKGROUND);

	for (int row = 0; row < enemies_rows; row++)
	{
		for (int col = 0; col < enemies_cols; col++)
		{
			agent_t& enem = m_enemies[row * enemies_cols + col];
			enem.current_frame = currframe;
		}
	}

}

void App::check_enemy_death()
{
	for (int row = 0; row < enemies_rows; row++)
	{
		for (int col = 0; col < enemies_cols; col++)
		{
			agent_t& enem = m_enemies[row * enemies_cols + col];
			if (IsWindowVisible(enem.window))
			{
				RECT enemRect = { enem.pos.x, enem.pos.y, enem.pos.x + m_enemy_size.x, enem.pos.y + m_enemy_size.y };
				// To jest głupie i bez sensu, co mi zrobicie?
				for (auto& bullet : m_bullets)
				{
					RECT bulletRect = { bullet.pos.x, bullet.pos.y, bullet.pos.x + bullet_size.x, bullet.pos.y + bullet_size.y };
					RECT destRect {};
					if (IsWindowVisible(bullet.window) && IntersectRect(&destRect, &enemRect, &bulletRect))
					{
						ShowWindow(enem.window, SW_HIDE);
						ShowWindow(bullet.window, SW_HIDE);
					}
				}
			}
		}
	}

}

void App::check_player_lost()
{
	for (int row = 0; row < enemies_rows; row++)
	{
		for (int col = 0; col < enemies_cols; col++)
		{
			agent_t& enem = m_enemies[row * enemies_cols + col];
			if (IsWindowVisible(enem.window))
			{
				RECT enemRect = { enem.pos.x, enem.pos.y, enem.pos.x + m_enemy_size.x, enem.pos.y + m_enemy_size.y };
				RECT playerRect = { m_player.pos.x, m_player.pos.y, m_player.pos.x + m_player_size.x, m_player.pos.y + m_player_size.y };
				RECT dest {};
				if (IntersectRect(&dest, &enemRect, &playerRect) || enemRect.top > m_height)
				{
					new_game();
				}
			}
		}
	}
}

void App::new_game()
{
	state = game_state::playing;

	LONG groupXoffset = (enemies_cols - 1) * (m_enemy_size.x / 2);
	LONG groupYoffset = (enemies_rows - 1) * (m_enemy_size.y / 2);

	for (int row = 0; row < enemies_rows; row++)
	{
		for (int col = 0; col < enemies_cols; col++)
		{
			agent_t &enem = m_enemies[row * enemies_cols + col];
			enem.pos.x = m_enemy_def_pos.x + m_enemy_size.x * col - groupXoffset;
			enem.pos.y = m_enemy_def_pos.y + m_enemy_size.y * row - groupYoffset;
		}
	}
	for (auto &enemy : m_enemies)
	{
		ShowWindow(enemy.window, SW_SHOW);
	}

	for (auto &bullet : m_bullets)
	{
		ShowWindow(bullet.window, SW_HIDE);
	}

	m_player.pos.x = m_player_def_pos.x;
	m_player.pos.y = m_player_def_pos.y;
	MoveWindow(m_player.window, m_player.pos.x, m_player.pos.y, m_player_size.x, m_player_size.y, true);
	ShowWindow(m_player.window, SW_SHOW);
}

void App::draw_background(agent_t &agent)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(agent.window, &ps);

	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, agent.background);

	RECT rect;
	GetClientRect(agent.window, &rect);
	BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);

	SelectObject(hdcMem, hOldBitmap);
	DeleteDC(hdcMem);
	EndPaint(agent.window, &ps);

	InvalidateRect(agent.window, NULL, true);
}

void App::capture_background(agent_t &agent)
{
	if (agent.background)
	{
		DeleteObject(agent.background);
		agent.background = NULL;
	}
	RECT mainRect;
	GetClientRect(m_main, &mainRect);
	HDC hdcParent = GetDC(m_main);
	HDC hdcMem = CreateCompatibleDC(hdcParent);
	agent.background = CreateCompatibleBitmap(hdcParent, mainRect.right - mainRect.left, mainRect.bottom - mainRect.top);
	
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, agent.background);

	BitBlt(hdcMem, 0, 0, mainRect.right, mainRect.bottom, hdcParent, 0, 0, SRCCOPY);
	COLORREF col = GetPixel(hdcMem, 0, 0);
	COLORREF white = BACKGROUND_COLOR;
	DWORD r = GetRValue(col);
	DWORD g = GetGValue(col);
	DWORD b = GetBValue(col);

	SelectObject(hdcMem, hOldBitmap);
	DeleteDC(hdcMem);
	ReleaseDC(m_main, hdcParent);
}