-- ============================================================
-- create_dimensions.sql
-- Dimensions du Data Warehouse UPES (schéma en étoile)
-- ============================================================

-- ── Dimension Temps ─────────────────────────────────────────
CREATE TABLE IF NOT EXISTS dim_temps (
    temps_id    SERIAL PRIMARY KEY,
    date_value  DATE        NOT NULL UNIQUE,
    jour        SMALLINT    NOT NULL,
    mois        SMALLINT    NOT NULL,
    trimestre   SMALLINT    NOT NULL,
    annee       SMALLINT    NOT NULL,
    semestre    SMALLINT    NOT NULL,
    nom_mois    VARCHAR(20) NOT NULL,
    jour_semaine VARCHAR(15) NOT NULL
);

-- ── Dimension Filière ────────────────────────────────────────
CREATE TABLE IF NOT EXISTS dim_filiere (
    filiere_id   SERIAL PRIMARY KEY,
    nom_filiere  VARCHAR(100) NOT NULL UNIQUE,
    departement  VARCHAR(100),
    niveau       VARCHAR(50)  DEFAULT 'Licence',
    duree_annees SMALLINT     DEFAULT 3
);

-- ── Dimension Étudiant ───────────────────────────────────────
CREATE TABLE IF NOT EXISTS dim_etudiant (
    etudiant_id      INTEGER PRIMARY KEY,
    nom              VARCHAR(150) NOT NULL,
    filiere_id       INTEGER REFERENCES dim_filiere(filiere_id),
    annee_inscription SMALLINT,
    statut           VARCHAR(30),
    date_naissance   DATE,
    email            VARCHAR(150),
    sexe             CHAR(1)
);

-- ── Dimension Enseignant ─────────────────────────────────────
CREATE TABLE IF NOT EXISTS dim_enseignant (
    enseignant_id    INTEGER PRIMARY KEY,
    nom              VARCHAR(150) NOT NULL,
    grade            VARCHAR(80),
    departement      VARCHAR(100),
    email            VARCHAR(150),
    date_recrutement DATE
);

-- ── Dimension Module ─────────────────────────────────────────
CREATE TABLE IF NOT EXISTS dim_module (
    module_id   SERIAL PRIMARY KEY,
    nom_module  VARCHAR(150) NOT NULL,
    filiere_id  INTEGER REFERENCES dim_filiere(filiere_id),
    credits     SMALLINT DEFAULT 3,
    coefficient NUMERIC(3,1) DEFAULT 1.5,
    UNIQUE (nom_module, filiere_id)
);

-- ── Dimension Salle ──────────────────────────────────────────
CREATE TABLE IF NOT EXISTS dim_salle (
    salle_id    INTEGER PRIMARY KEY,
    nom_salle   VARCHAR(50)  NOT NULL,
    capacite    SMALLINT     NOT NULL,
    type        VARCHAR(50),
    batiment    VARCHAR(50),
    disponible  BOOLEAN      DEFAULT TRUE
);
