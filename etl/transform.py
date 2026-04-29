"""
transform.py
Transformations et nettoyage des données CSV brutes
avant chargement dans le Data Warehouse PostgreSQL.
"""

import os
import pandas as pd
from datetime import date

DATA_DIR = os.path.join(os.path.dirname(__file__), "..", "data")


def load_csv(filename: str) -> pd.DataFrame:
    path = os.path.join(DATA_DIR, filename)
    df = pd.read_csv(path, encoding="utf-8")
    print(f"  Chargé : {filename} ({len(df)} lignes)")
    return df


# ── Dimension Temps ───────────────────────────────────────────────────────────
def build_dim_temps(date_series: pd.Series) -> pd.DataFrame:
    """Construit la dimension temps à partir d'une série de dates."""
    dates = pd.to_datetime(date_series.dropna()).dt.date
    unique_dates = sorted(set(dates))
    rows = []
    for d in unique_dates:
        dt = pd.Timestamp(d)
        rows.append({
            "date_value": d,
            "jour": dt.day,
            "mois": dt.month,
            "trimestre": (dt.month - 1) // 3 + 1,
            "annee": dt.year,
            "semestre": 1 if dt.month <= 6 else 2,
            "nom_mois": dt.strftime("%B"),
            "jour_semaine": dt.strftime("%A"),
        })
    df = pd.DataFrame(rows)
    df.insert(0, "temps_id", range(1, len(df) + 1))
    return df


# ── Dimension Filière ─────────────────────────────────────────────────────────
def build_dim_filiere(df_etudiants: pd.DataFrame) -> pd.DataFrame:
    filieres = df_etudiants["filiere"].dropna().unique()
    rows = [
        {
            "filiere_id": i + 1,
            "nom_filiere": f,
            "departement": f,
            "niveau": "Licence",
            "duree_annees": 3,
        }
        for i, f in enumerate(sorted(filieres))
    ]
    return pd.DataFrame(rows)


# ── Dimension Étudiant ────────────────────────────────────────────────────────
def build_dim_etudiant(df_etudiants: pd.DataFrame, df_filieres: pd.DataFrame) -> pd.DataFrame:
    df = df_etudiants.copy()
    filiere_map = df_filieres.set_index("nom_filiere")["filiere_id"].to_dict()
    df["filiere_id"] = df["filiere"].map(filiere_map)
    df["date_naissance"] = pd.to_datetime(df["date_naissance"], errors="coerce").dt.date
    return df[["etudiant_id", "nom", "filiere_id", "annee_inscription", "statut",
               "date_naissance", "email", "sexe"]]


# ── Dimension Enseignant ──────────────────────────────────────────────────────
def build_dim_enseignant(df_enseignants: pd.DataFrame) -> pd.DataFrame:
    df = df_enseignants.copy()
    df["date_recrutement"] = pd.to_datetime(df["date_recrutement"], errors="coerce").dt.date
    return df[["enseignant_id", "nom", "grade", "departement", "email", "date_recrutement"]]


# ── Dimension Module ──────────────────────────────────────────────────────────
def build_dim_module(df_notes: pd.DataFrame, df_filieres: pd.DataFrame) -> pd.DataFrame:
    filiere_map = df_filieres.set_index("nom_filiere")["filiere_id"].to_dict()
    pairs = df_notes[["module", "filiere"]].drop_duplicates().dropna()
    pairs = pairs.rename(columns={"module": "nom_module"})
    pairs["filiere_id"] = pairs["filiere"].map(filiere_map)
    pairs = pairs.drop(columns=["filiere"]).drop_duplicates().reset_index(drop=True)
    pairs.insert(0, "module_id", range(1, len(pairs) + 1))
    pairs["credits"] = 3
    pairs["coefficient"] = 1.5
    return pairs


# ── Dimension Salle ───────────────────────────────────────────────────────────
def build_dim_salle(df_salles: pd.DataFrame) -> pd.DataFrame:
    df = df_salles.copy()
    df["disponible"] = df["disponible"].map(
        {True: True, False: False, "True": True, "False": False}
    ).fillna(True)
    return df[["salle_id", "nom_salle", "capacite", "type", "batiment", "disponible"]]


# ── Fait : Inscriptions ───────────────────────────────────────────────────────
def build_fact_inscriptions(df_etudiants: pd.DataFrame, df_filieres: pd.DataFrame,
                             df_temps: pd.DataFrame) -> pd.DataFrame:
    filiere_map = df_filieres.set_index("nom_filiere")["filiere_id"].to_dict()
    temps_map = {
        str(r["date_value"]): r["temps_id"]
        for _, r in df_temps.iterrows()
    }

    rows = []
    for _, row in df_etudiants.iterrows():
        # Référence temporelle : 1er septembre de l'année d'inscription
        ref_date = date(int(row["annee_inscription"]), 9, 1)
        rows.append({
            "etudiant_id": row["etudiant_id"],
            "filiere_id": filiere_map.get(row["filiere"]),
            "temps_id": temps_map.get(str(ref_date)),
            "annee": row["annee_inscription"],
            "statut": row["statut"],
            "nb_inscriptions": 1,
        })
    df = pd.DataFrame(rows).reset_index(drop=True)
    df.insert(0, "inscription_id", range(1, len(df) + 1))
    return df


# ── Fait : Notes ──────────────────────────────────────────────────────────────
def build_fact_notes(df_notes: pd.DataFrame, df_modules: pd.DataFrame,
                     df_filieres: pd.DataFrame, df_temps: pd.DataFrame) -> pd.DataFrame:
    module_map = df_modules.set_index("nom_module")["module_id"].to_dict()
    filiere_map = df_filieres.set_index("nom_filiere")["filiere_id"].to_dict()

    # Mapper temps sur (annee, semestre) → prendre le 1er jour du semestre
    def sem_date(annee, semestre):
        return date(int(annee), 2 if semestre == 1 else 9, 1)

    temps_map = {}
    for _, r in df_temps.iterrows():
        key = str(r["date_value"])
        temps_map[key] = r["temps_id"]

    rows = []
    for _, row in df_notes.iterrows():
        ref_date = sem_date(row["annee"], row["semestre"])
        rows.append({
            "note_id": row["note_id"],
            "etudiant_id": row["etudiant_id"],
            "module_id": module_map.get(row["module"]),
            "filiere_id": filiere_map.get(row["filiere"]),
            "temps_id": temps_map.get(str(ref_date)),
            "annee": row["annee"],
            "semestre": row["semestre"],
            "note": row["note"],
            "mention": row["mention"],
        })
    return pd.DataFrame(rows)


# ── Fait : Finances ───────────────────────────────────────────────────────────
def build_fact_finances(df_finances: pd.DataFrame, df_temps: pd.DataFrame) -> pd.DataFrame:
    temps_map = {
        str(r["date_value"]): r["temps_id"]
        for _, r in df_temps.iterrows()
    }
    df = df_finances.copy()
    df["temps_id"] = pd.to_datetime(df["date"], errors="coerce").dt.date.apply(
        lambda d: temps_map.get(str(d))
    )
    return df[["finance_id", "temps_id", "annee", "poste_budgetaire",
               "type", "montant", "departement", "description"]]


# ── Fait : Recherche ──────────────────────────────────────────────────────────
def build_fact_recherche(df_recherche: pd.DataFrame, df_temps: pd.DataFrame) -> pd.DataFrame:
    temps_map = {
        str(r["date_value"]): r["temps_id"]
        for _, r in df_temps.iterrows()
    }
    df = df_recherche.copy()
    df["temps_debut_id"] = pd.to_datetime(df["date_debut"], errors="coerce").dt.date.apply(
        lambda d: temps_map.get(str(d))
    )
    return df[["projet_id", "enseignant_id", "temps_debut_id", "departement",
               "thematique", "budget", "depenses", "statut", "publications"]]


# ── Pipeline complet de transformation ───────────────────────────────────────
def run_transformations():
    print("=== Transformation des données ===")

    df_etudiants  = load_csv("etudiants.csv")
    df_enseignants = load_csv("enseignants.csv")
    df_notes      = load_csv("notes.csv")
    df_finances   = load_csv("finances.csv")
    df_salles     = load_csv("salles.csv")
    df_recherche  = load_csv("recherche.csv")

    # Dates utilisées pour la dimension temps
    all_dates = pd.concat([
        pd.to_datetime(df_finances["date"], errors="coerce"),
        pd.to_datetime(df_recherche["date_debut"], errors="coerce"),
    ]).dropna()

    # Ajouter les 1ers septembre de chaque année d'inscription
    import datetime
    extra = [pd.Timestamp(datetime.date(y, m, 1))
             for y in range(2018, 2026)
             for m in (2, 9)]
    all_dates = pd.concat([all_dates, pd.Series(extra)]).drop_duplicates()

    dim_temps        = build_dim_temps(all_dates)
    dim_filiere      = build_dim_filiere(df_etudiants)
    dim_etudiant     = build_dim_etudiant(df_etudiants, dim_filiere)
    dim_enseignant   = build_dim_enseignant(df_enseignants)
    dim_module       = build_dim_module(df_notes, dim_filiere)
    dim_salle        = build_dim_salle(df_salles)
    fact_inscriptions = build_fact_inscriptions(df_etudiants, dim_filiere, dim_temps)
    fact_notes        = build_fact_notes(df_notes, dim_module, dim_filiere, dim_temps)
    fact_finances     = build_fact_finances(df_finances, dim_temps)
    fact_recherche    = build_fact_recherche(df_recherche, dim_temps)

    print("\nTransformations terminées. Tables prêtes au chargement.")
    return {
        "dim_temps":         dim_temps,
        "dim_filiere":       dim_filiere,
        "dim_etudiant":      dim_etudiant,
        "dim_enseignant":    dim_enseignant,
        "dim_module":        dim_module,
        "dim_salle":         dim_salle,
        "fact_inscriptions": fact_inscriptions,
        "fact_notes":        fact_notes,
        "fact_finances":     fact_finances,
        "fact_recherche":    fact_recherche,
    }


if __name__ == "__main__":
    tables = run_transformations()
    for name, df in tables.items():
        print(f"  {name}: {df.shape[0]} lignes x {df.shape[1]} colonnes")
