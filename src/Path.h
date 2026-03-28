#pragma once

class Path {
public:
	Path(glm::vec2 pos = glm::vec2(0, 0), float sm = 1, int tipo = 0)
	{
		m_pos = pos;
		m_speedMultiplier = sm;
		m_tipo = tipo;
	}
	Path* getNextPath() { return m_nextPath; }
	void setPreviousPath(Path* path) { m_prevPath = path; }
	void setNextPath(Path* path) { m_nextPath = path; }
	void setspeedMultiplier(float vel) { m_speedMultiplier = vel; }
	void calculateBisector()
	{
		if (m_prevPath && m_nextPath)
		{
			glm::vec2 dir1 = glm::normalize(m_pos - m_prevPath->m_pos);
			glm::vec2 dir2 = glm::normalize(m_nextPath->m_pos - m_pos);

			glm::vec2 bis = glm::normalize(dir1 + dir2);

			m_bisectorNormal = bis;
		}
		else
		{
			m_bisectorNormal = glm::vec2(1, 0);
		}
	}
	glm::vec2 getBisector() { return m_bisectorNormal; }
	glm::vec2 getPos() { return m_pos; }
	int getTipus() { return m_tipo; }
	float getSpeedMultiplier() { return m_speedMultiplier; }
	Path* getPrevPath() { return m_prevPath; }

private:
	float m_speedMultiplier = 1;
	glm::vec2 m_pos;
	int m_tipo;


	Path* m_prevPath = nullptr;
	Path* m_nextPath = nullptr;

	glm::vec2 m_bisectorNormal = glm::vec2(0, 0);
};