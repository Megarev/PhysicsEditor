#pragma once
#include "polygon.h"
#include "constraints.h"

class Level {
private:
	Level() {}
public:
	static Level Get() {
		static Level level;
		return level;
	}

	void DemoScene(std::vector<PolygonShape>& polygons, ConstraintManager& constraint_mgr, JointPairManager& joint_mgr, uint32_t& id_count) {

		auto AddPolygon = [&](int n_vertices, const olc::vf2d& pos, const olc::Pixel& color, const std::vector<olc::vf2d>& vertices,
			float mass, float e, float sf, float df, float w = 0.0f) -> void {
				polygons.push_back(PolygonShape{ n_vertices, olc::vf2d{ 1.0f, 1.0f }, pos, color, id_count++ });
				for (int i = 0; i < polygons.back().n_vertices; i++) {
					polygons.back().GetVertex(i) = vertices[i];
				}
				polygons.back().properties.mass = mass;
				polygons.back().properties.e = e;
				polygons.back().properties.sf = sf;
				polygons.back().properties.df = df;
				polygons.back().properties.angular_velocity = w;
		};

		AddPolygon(4, { 162.0f, 420.0f }, olc::DARK_CYAN, {
			{ 471.301, 442.95 },
			{ 139.051, 442.301 },
			{ 139.699, 397.05 },
			{ 472.95, 398.699 }
			}, 0.0f, 1.0f, 0.5f, 0.2f, 0.0f
		);

		AddPolygon(4, { 199.0f, 375.0f }, olc::CYAN, {
			{ 221.627, 397.627 },
			{ 176.373, 397.627 },
			{ 176.373, 352.373 },
			{ 221.627, 352.373 },
			}, 10.0f, 1.0f, 0.5f, 0.2f, 0.0f
			);

		AddPolygon(4, { 408.0f, 376.0f }, olc::CYAN, {
			{ 430.627, 398.627 },
			{ 385.373, 398.627 },
			{ 385.373, 353.373 },
			{ 430.627, 353.373 },
			}, 10.0f, 1.0f, 0.5f, 0.2f, 0.0f
			);

		AddPolygon(4, { 200.0f, 332.0f }, olc::CYAN * 0.8f, {
			{ 430.731, 351.523 },
			{ 177.477, 354.731 },
			{ 177.269, 309.477 },
			{ 430.523, 309.269 },
			}, 8.0f, 1.0f, 0.5f, 0.2f, 0.0f
			);

		AddPolygon(4, { 246.0f, 287.0f }, olc::CYAN * 0.6f, {
			{ 268.627, 309.627 },
			{ 223.373, 309.627 },
			{ 223.373, 264.373 },
			{ 268.627, 264.373 },
			}, 5.0f, 1.0f, 0.5f, 0.2f, 0.0f
			);

		AddPolygon(4, { 360.0f, 285.0f }, olc::CYAN * 0.6f, {
			{ 382.627, 307.627 },
			{ 337.373, 307.627 },
			{ 337.373, 262.373 },
			{ 382.627, 262.373 },
			}, 5.0f, 1.0f, 0.5f, 0.2f, 0.0f
			);

		AddPolygon(4, { 312.0f, 237.0f }, olc::CYAN * 0.5f, {
			{ 387.567, 263.996 },
			{ 221.307, 261.386 },
			{ 260.422, 218.235 },
			{ 355.687, 220.619 },
			}, 2.0f, 1.0f, 0.5f, 0.2f, 0.0f
			);

		AddPolygon(3, { 306.0f, 202.0f }, olc::YELLOW, {
			{ 332.924, 219.295 },
			{ 277.56, 216.669 },
			{ 307.516, 170.036 },
			}, 1.0f, 1.0f, 0.5f, 0.2f, 0.0f
			);

		AddPolygon(10, { 513.0f, 22.0f }, olc::GREEN, {
			{ 536.627f, 44.6274f },
			{ 519.006f, 53.606f },
			{ 499.472f, 50.5122f },
			{ 485.488f, 36.5277f },
			{ 482.394f, 16.9941f },
			{ 491.373f, -0.627411f },
			{ 508.994f, -9.60602f },
			{ 528.528f, -6.51221f },
			{ 542.512f, 7.47229f },
			{ 545.606f, 27.0059f },
			}, 1.0f, 1.0f, 0.5f, 0.2f, 1.0f
			);

		AddPolygon(10, { 612.0f, 32.0f }, olc::MAGENTA, {
			{ 634.627f, 54.6274f },
			{ 617.006f, 63.606f },
			{ 597.472f, 60.5122f },
			{ 583.488f, 46.5277f },
			{ 580.394f, 26.9941f },
			{ 589.373f, 9.37259f },
			{ 606.994f, 0.393982f },
			{ 626.528f, 3.48779f },
			{ 640.512f, 17.4723f },
			{ 643.606f, 37.0059f },
			}, 25.0f, 1.0f, 0.5f, 0.2f, 0.0f
			);


		constraint_mgr.constraints_data.push_back({ 8, { { 513.0f, 15.0f }, { 417.0f, 23.0f } }, 0.5f, 0.02f });
		joint_mgr.data.push_back({ { 8, 9 }, { { 612.0f, 32.0f }, { 513.0f, 22.0f } }, 0.5f, 0.02f });
	}
};