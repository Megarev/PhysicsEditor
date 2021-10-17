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

		AddPolygon(10, { 392.0f, 36.0f }, olc::MAGENTA, {
			{ 414.627f, 58.627f },
			{ 397.006f, 67.606f },
			{ 377.472f, 64.512f },
			{ 363.488f, 50.528f },
			{ 360.394f, 30.994f },
			{ 369.373f, 13.3726f },
			{ 386.994f, 4.394f },
			{ 406.528f, 7.4878f },
			{ 420.512f, 21.472f },
			{ 423.606f, 41.006f }
			}, 50.0f, 1.0f, 0.5f, 0.2f, 0.0f
		);

		AddPolygon(10, { 465.0f, 78.0f }, olc::GREEN, {
			{ 487.627f, 100.627f },
			{ 470.006f, 109.606f },
			{ 450.472f, 106.512f },
			{ 436.488f, 92.5277f },
			{ 433.394f, 72.9941f },
			{ 442.373f, 55.3726f },
			{ 459.994f, 46.394f },
			{ 479.528f, 49.4878f },
			{ 493.512f, 63.4723f },
			{ 496.606f, 83.0059f }
			}, 10.0f, 1.0f, 0.5f, 0.2f, 1.0f
			);

		constraint_mgr.constraints_data.push_back({ 8, { { 392.0f, 36.0f }, { 287.0f, 62.0f } }, 0.5f, 0.02f });
		joint_mgr.data.push_back({ { 8, 9 }, { { 392.0f, 36.0f }, { 465.0f, 78.0f } }, 0.5f, 0.02f });
	}
};