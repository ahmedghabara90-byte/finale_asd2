-- ============================================================
-- create_facts.sql
-- Tables de faits du Data Warehouse UPES
-- ============================================================

-- ── Fait : Inscriptions ──────────────────────────────────────
CREATE TABLE IF NOT EXISTS fact_inscriptions (
    inscription_id  SERIAL PRIMARY KEY,
    etudiant_id     INTEGER REFERENCES dim_etudiant(etudiant_id),
    filiere_id      INTEGER REFERENCES dim_filiere(filiere_id),
    temps_id        INTEGER REFERENCES dim_temps(temps_id),
    annee           SMALLINT NOT NULL,
    statut          VARCHAR(30),
    nb_inscriptions INTEGER DEFAULT 1
);

-- ── Fait : Notes ─────────────────────────────────────────────
CREATE TABLE IF NOT EXISTS fact_notes (
    note_id      SERIAL PRIMARY KEY,
    etudiant_id  INTEGER  REFERENCES dim_etudiant(etudiant_id),
    module_id    INTEGER  REFERENCES dim_module(module_id),
    filiere_id   INTEGER  REFERENCES dim_filiere(filiere_id),
    temps_id     INTEGER  REFERENCES dim_temps(temps_id),
    annee        SMALLINT NOT NULL,
    semestre     SMALLINT NOT NULL,
    note         NUMERIC(4,2) CHECK (note >= 0 AND note <= 20),
    mention      VARCHAR(20),
    est_reussi   BOOLEAN  GENERATED ALWAYS AS (note >= 10) STORED
);

-- ── Fait : Finances ──────────────────────────────────────────
CREATE TABLE IF NOT EXISTS fact_finances (
    finance_id        SERIAL PRIMARY KEY,
    temps_id          INTEGER REFERENCES dim_temps(temps_id),
    annee             SMALLINT NOT NULL,
    poste_budgetaire  VARCHAR(150),
    type              VARCHAR(20) CHECK (type IN ('Dépense', 'Recette')),
    montant           NUMERIC(15,2) NOT NULL,
    departement       VARCHAR(100),
    description       TEXT
);

-- ── Fait : Recherche scientifique ────────────────────────────
CREATE TABLE IF NOT EXISTS fact_recherche (
    projet_id        SERIAL PRIMARY KEY,
    enseignant_id    INTEGER REFERENCES dim_enseignant(enseignant_id),
    temps_debut_id   INTEGER REFERENCES dim_temps(temps_id),
    departement      VARCHAR(100),
    thematique       VARCHAR(150),
    budget           NUMERIC(15,2),
    depenses         NUMERIC(15,2),
    statut           VARCHAR(30),
    publications     SMALLINT DEFAULT 0
);
