-- ============================================================
-- create_views.sql
-- Vues matérialisées pour les KPIs UPES
-- ============================================================

-- ── KPI 1 : Taux de réussite par filière et par semestre ────
CREATE MATERIALIZED VIEW IF NOT EXISTS kpi_taux_reussite AS
SELECT
    f.nom_filiere,
    fn.annee,
    fn.semestre,
    COUNT(*)                                              AS total_notes,
    SUM(CASE WHEN fn.est_reussi THEN 1 ELSE 0 END)       AS notes_reussies,
    ROUND(
        100.0 * SUM(CASE WHEN fn.est_reussi THEN 1 ELSE 0 END) / NULLIF(COUNT(*), 0),
        2
    )                                                     AS taux_reussite_pct
FROM fact_notes fn
JOIN dim_filiere f ON fn.filiere_id = f.filiere_id
GROUP BY f.nom_filiere, fn.annee, fn.semestre
ORDER BY fn.annee, fn.semestre, f.nom_filiere;

-- ── KPI 2 : Taux d'abandon par année ─────────────────────────
CREATE MATERIALIZED VIEW IF NOT EXISTS kpi_taux_abandon AS
SELECT
    fi.annee,
    f.nom_filiere,
    COUNT(*)                                                         AS total_inscrits,
    SUM(CASE WHEN fi.statut = 'Abandonné' THEN 1 ELSE 0 END)       AS abandons,
    ROUND(
        100.0 * SUM(CASE WHEN fi.statut = 'Abandonné' THEN 1 ELSE 0 END) / NULLIF(COUNT(*), 0),
        2
    )                                                                AS taux_abandon_pct
FROM fact_inscriptions fi
JOIN dim_filiere f ON fi.filiere_id = f.filiere_id
GROUP BY fi.annee, f.nom_filiere
ORDER BY fi.annee, f.nom_filiere;

-- ── KPI 3 : Nombre d'inscrits par filière ────────────────────
CREATE MATERIALIZED VIEW IF NOT EXISTS kpi_inscrits_par_filiere AS
SELECT
    f.nom_filiere,
    fi.annee,
    COUNT(*)  AS nb_inscrits,
    SUM(CASE WHEN fi.statut = 'Diplômé'  THEN 1 ELSE 0 END) AS diplomes,
    SUM(CASE WHEN fi.statut = 'Inscrit'  THEN 1 ELSE 0 END) AS actifs,
    SUM(CASE WHEN fi.statut = 'Abandonné' THEN 1 ELSE 0 END) AS abandonnes
FROM fact_inscriptions fi
JOIN dim_filiere f ON fi.filiere_id = f.filiere_id
GROUP BY f.nom_filiere, fi.annee
ORDER BY fi.annee, f.nom_filiere;

-- ── KPI 4 : Budget vs Dépenses réelles ───────────────────────
CREATE MATERIALIZED VIEW IF NOT EXISTS kpi_budget_depenses AS
SELECT
    ff.annee,
    ff.departement,
    SUM(CASE WHEN ff.type = 'Recette'  THEN ff.montant ELSE 0 END) AS total_recettes,
    SUM(CASE WHEN ff.type = 'Dépense'  THEN ff.montant ELSE 0 END) AS total_depenses,
    SUM(CASE WHEN ff.type = 'Recette'  THEN ff.montant ELSE 0 END)
    - SUM(CASE WHEN ff.type = 'Dépense' THEN ff.montant ELSE 0 END) AS solde
FROM fact_finances ff
GROUP BY ff.annee, ff.departement
ORDER BY ff.annee, ff.departement;

-- ── KPI 5 : Ratio étudiants / enseignants par département ────
CREATE MATERIALIZED VIEW IF NOT EXISTS kpi_ratio_etudiant_enseignant AS
SELECT
    f.nom_filiere                          AS departement,
    fi.annee,
    COUNT(DISTINCT fi.etudiant_id)         AS nb_etudiants,
    COUNT(DISTINCT e.enseignant_id)        AS nb_enseignants,
    ROUND(
        COUNT(DISTINCT fi.etudiant_id)::NUMERIC
        / NULLIF(COUNT(DISTINCT e.enseignant_id), 0),
        1
    )                                      AS ratio
FROM fact_inscriptions fi
JOIN dim_filiere f  ON fi.filiere_id   = f.filiere_id
LEFT JOIN dim_enseignant e ON e.departement = f.nom_filiere
GROUP BY f.nom_filiere, fi.annee
ORDER BY fi.annee, f.nom_filiere;

-- ── KPI 6 : Publications par département ─────────────────────
CREATE MATERIALIZED VIEW IF NOT EXISTS kpi_publications_departement AS
SELECT
    fr.departement,
    SUM(fr.publications)    AS total_publications,
    COUNT(*)                AS nb_projets,
    SUM(fr.budget)          AS budget_total,
    SUM(fr.depenses)        AS depenses_totales,
    ROUND(
        100.0 * SUM(fr.depenses) / NULLIF(SUM(fr.budget), 0),
        2
    )                       AS taux_consommation_pct
FROM fact_recherche fr
GROUP BY fr.departement
ORDER BY total_publications DESC;

-- ── Index pour performances ───────────────────────────────────
CREATE INDEX IF NOT EXISTS idx_fact_notes_filiere   ON fact_notes(filiere_id);
CREATE INDEX IF NOT EXISTS idx_fact_notes_annee     ON fact_notes(annee);
CREATE INDEX IF NOT EXISTS idx_fact_inscriptions_an ON fact_inscriptions(annee);
CREATE INDEX IF NOT EXISTS idx_fact_finances_annee  ON fact_finances(annee);
CREATE INDEX IF NOT EXISTS idx_fact_recherche_dept  ON fact_recherche(departement);
